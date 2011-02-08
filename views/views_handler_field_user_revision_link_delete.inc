<?php
// $Id:
/**
 * Field handler to present delete a user revision.
 */
class views_handler_field_user_revision_link_delete extends views_handler_field_user_link {
  function construct() {
    parent::construct();
    $this->additional_fields['authorid'] = array('table' => 'user', 'field' => 'authorid');
    $this->additional_fields['user_vid'] = array('table' => 'user', 'field' => 'vid');
    $this->additional_fields['vid'] = 'vid';
  }

  function access() {
    return user_access('delete user revisions') || user_access('administer users');
  }

  function render_link($data, $values) {
    if (!this->access()) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');

    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = "user/$user->nid/revisions/$user->vid/delete";
    $this->options['alter']['query'] = drupal_get_destination();

    return $text;
  }
}
