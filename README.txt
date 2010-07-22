Description
-----------
The taxonomy_revisions module extends Drupal's revisioning system to taxonomy terms, using the same familiar UI. In a future release, you will be able to make revisions of vocabularies too, in order to keep track of changes to both structure and content. 

Installation
------------
Install taxonomy_revisions as you would any drupal module. You can download the module from drupal.org and place it in your modules directory. Alternatively, if you have drush installed, you can type:

drush dl taxonomy_revisions

You then must enable the module by pointing your browser to http://www.example.com/admin/build/modules/list and clicking the check box for the flat_book module. With drush you can type:

drush en taxonomy_revisions

Configuration
-------------
You can access the configuration page at http://www.example/com/admin/settings/taxonomy_revisions. At present there is only one option which is configurable. You may choose to disable the edit link that is provided on taxonomy terms (enabled by default). This was added to standardize the interface between pieces of content with revisions, but may not be appropriate in all contexts.

Author
------
Jonathan Pullano [critical_patch]
Project sponsored by Evolving Web

Support
-------
If you experience problems with the taxonomy_revisions module, please post in the module issue queue. DO NOT post in the forums, it is very unlikely that I will see your post. The author and Evolving Web are not responsible for any damage done to you site or data lost through the use of the taxonomy_revisions module.
