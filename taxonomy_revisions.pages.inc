<?php

function taxonomy_revisions_revision_overview($tid) {
  $term = taxonomy_get_term($tid);

  drupal_set_title(t('Revisions for %title', array('%title' => $term->name)));

  $header = array(t('Revision'), array('data' => t('Operations'), 'colspan' => 2));

  $revisions = term_revision_list($term);

  $rows = array();
  $revert_permission = FALSE;
  //Unlike with node revisions, we don't have to worry about "restricted formats"
  if ((user_access('revert revisions') || user_access('administer taxonomy'))) {
    $revert_permission = TRUE;
  }
  $delete_permission = FALSE;
  if ((user_access('delete revisions') || user_access('administer taxonomy'))) {
    $delete_permission = TRUE;
  }
  foreach ($revisions as $revision) {
    $row = array();
    $operations = array();

    if ($revision->current) {
      $row[] = array('data' => t('!date by !username', array('!date' => l(_taxonomy_revisions_format_date($revision->timestamp, 'small'), "taxonomy/term/$tid"), '!username' => theme('username', $revision)))
                               . (($revision->log != '') ? '<p class="revision-log">'. filter_xss($revision->log) .'</p>' : ''),
                     'class' => 'revision-current');
      $operations[] = array('data' => theme('placeholder', t('current revision')), 'class' => 'revision-current', 'colspan' => 2);
    }
    else {
      $row[] = t('!date by !username', array('!date' => l(_taxonomy_revisions_format_date($revision->timestamp, 'small'), "taxonomy/term/$tid/revisions/$revision->tvid/view"), '!username' => theme('username', $revision)))
               . (($revision->log != '') ? '<p class="revision-log">'. filter_xss($revision->log) .'</p>' : '');
      if ($revert_permission) {
        $operations[] = l(t('revert'), "taxonomy/term/$tid/revisions/$revision->tvid/revert");
      }
      if ($delete_permission) {
        $operations[] = l(t('delete'), "taxonomy/term/$tid/revisions/$revision->tvid/delete");
      }
    }
    $rows[] = array_merge($row, $operations);
  }

  return theme('table', $header, $rows);
}

/**
 * Ask for confirmation of the reversion to prevent against CSRF attacks.
 */
function term_revision_revert_confirm($form_state, $tid, $tvid) {
  $form['#term_revision'] = _taxonomy_revisions_get_term_revision($tvid);
  return confirm_form($form, t('Are you sure you want to revert to the revision from %revision-date?', array('%revision-date' => _taxonomy_revisions_format_date($form['#term_revision']->timestamp))), 'taxonomy/term/'. $form['#term_revision']->tid .'/revisions', '', t('Revert'), t('Cancel'));
}

function term_revision_revert_confirm_submit($form, &$form_state) {
  global $user;
  $term_revision = $form['#term_revision'];
  $term_revision->log = t('Copy of the revision from %date.', array('%date' => _taxonomy_revisions_format_date($term_revision->timestamp)));
  $term_revision->timestamp = time();
  $term_revision->uid = $user->uid;

  taxonomy_revisions_term_save_revision($term_revision); 

  watchdog('content', 'term: reverted %name revision %revision.', array('%name' => $term_revision->name, '%revision' => $term_revision->tvid));
  drupal_set_message(t('term %name has been reverted back to the revision from %revision-date.', array('%name' => $term_revision->name, '%revision-date' => _taxonomy_revisions_format_date($term_revision->timestamp))));
  $form_state['redirect'] = 'taxonomy/term/'. $term_revision->tid .'/revisions';
}

function term_revision_delete_confirm($form_state, $tid, $tvid) {
  $form['#term_revision'] = _taxonomy_revisions_get_term_revision($tvid);
  return confirm_form($form, t('Are you sure you want to delete the revision from %revision-date?', array('%revision-date' => _taxonomy_revisions_format_date($term_revision->timestamp))), 'taxonomy/term/'. $term_revision->tid .'/revisions', t('This action cannot be undone.'), t('Delete'), t('Cancel'));
} 

function term_revision_delete_confirm_submit($form, &$form_state) {
  $term_revision = $form['#term_revision'];
  //We can never delete the current revision, so all revisions will be in the term revisions table
  db_query("DELETE FROM {term_revisions} WHERE tid = %d AND tvid = %d", $term_revision->tid, $term_revision->tvid);
  module_invoke_all('delete_term_revision', $term_revision);
  watchdog('content', 'term: deleted %name revision %revision.', array('%name' => $term_revision->name, '%revision' => $term_revision->tvid));
  drupal_set_message(t('Revision from %revision-date of term %name has been deleted.', array('%revision-date' => _taxonomy_revisions_format_date($term_revision->timestamp), '%name' => $term_revision->name)));
  $form_state['redirect'] = 'taxonomy/term/'. $term_revision->tid;
  if (db_result(db_query('SELECT COUNT(tvid) FROM {term_revisions} WHERE tid = %d', $term_revision->tid)) > 1) {
    $form_state['redirect'] .= '/revisions';
  }
}

function term_show_revision($tid, $tvid) {
  $revision = _taxonomy_revisions_get_term_revision($tvid);
  $current = taxonomy_get_term($tid);
  drupal_set_title(t('Revision of %name from %date', array('%name' => $current->name, '%date' => format_date($revision->timestamp))));

  $breadcrumb = array();
  //@todo taxonomy_get_parents will need to be fixed for vocabulary revision
  while ($parents = taxonomy_get_parents($current->tid)) {
    $current = array_shift($parents);
    $breadcrumb[] = l($current->name, 'taxonomy/term/'. $current->tid);
  }
  $breadcrumb[] = l(t('Home'), NULL);
  $breadcrumb = array_reverse($breadcrumb);
  drupal_set_breadcrumb($breadcrumb);
  
  //@todo taxonomy_select_nodes will need to be fixed for vocabulary revisions
  $output = theme('taxonomy_revisions_term_revision_page', $revision, taxonomy_select_nodes(array($tid), 'or', 0, TRUE));
  return $output;
}
