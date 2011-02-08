<?php
// $Id: 
/**
 * Field handler to present a link to revert a user to a revision
 */
class views_handler_field_user_revision_link_revert extends views_handler_field_user_link {
  function construct() {
    parent::construct();
    $this->additional_fields['authorid'] = array('table' => 'users', 'field' => 'uid');
    $this->additional_fields['user_vid'] = array('table' => 'users', 'field' => 'vid');
    $this->additional_fields['vid'] = 'vid';
  }

  function access() {
    return user_access('revert user revisions') || user_access('administer users');
  }

  function render_link($data, $values) {
    if (!this->access()) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('revert');

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "user/$user->nid/revisions/$user->vid/revert";
    $this->options['alter']['query'] = drupal_get_destination();

    return $text;
  }
}
