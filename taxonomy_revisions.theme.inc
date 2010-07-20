<?php

function theme_taxonomy_revisions_term_revision_page($revision, $result) {
  drupal_add_css(drupal_get_path('module', 'taxonomy') .'/taxonomy.css');

  $output = '<br/><h2 class="taxonomy-term-name">';
  $output .= check_plain($revision->name);
  $output .= '</h2>';

  $description = $revision->description;
  // Check that a description is set.
  if (!empty($description)) {
    $output .= '<div class="taxonomy-term-description">';
    $output .= filter_xss_admin($description);
    $output .= '</div>';
  }

  $output .= taxonomy_render_nodes($result);

  return $output;
}
