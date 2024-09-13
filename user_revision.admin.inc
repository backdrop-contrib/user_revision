<?php
/**
 * @file
 * Admin settings for user revision.
 */

/**
 * Form builder; Configure user revision settings for this site.
 */
function user_revision_admin_settings() {
  $config = config('user_revision.settings');
  $form['#config'] = 'user_revision.settings';
  $form['user_revision_by_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use revisions by default'),
    '#default_value' => $config->get('user_revision_by_default'),
    '#description' => t('If checked every save will create a new revision (can be overridden by users with %choose permission).', 
      array('%choose' => user_revision_permission() ['choose user revisions'] ['title'])),
  );
  $form['user_revision_admin_theme'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use administration theme'),
    '#default_value' => $config->get('user_revision_admin_theme'),
    '#description' => t('If checked revision pages will be shown using the administration theme (for users with appropriate permission).'),
  );
  return system_settings_form($form);
}
