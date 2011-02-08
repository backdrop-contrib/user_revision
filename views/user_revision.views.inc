<?php
// $Id:
/**
 * @file
 * Provide views data and handlers for user_revision.module
 */

/**
 * Implements hook_views_data()
 */
function user_revision_views_data() {

  // ----------------------------------------------------------------------
  // User revision table

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['user_revision']['table']['group']  = t('User revision');

  // Advertise this table as a possible base table
  $data['user_revision']['table']['base'] = array(
    'field' => 'vid',
    'title' => t('User revision'),
    'help' => t('User revision are a history of changes to nodes.'),
  );

  // For other base tables, explain how we join
  $data['user_revision']['table']['join'] = array(
    // Directly links to node table.
    'users' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  // uid field for node revision
  $data['user_revision']['authorid'] = array(
    'title' => t('User'),
    'help' => t('Relate a node revision to the user who created the revision.'),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'label' => t('revision user'),
    ),
  );

  // nid
  $data['user_revision']['vid'] = array(
    'title' => t('Vid'),
    'help' => t('The revision ID of the user revision.'),
    'field' => array(
      'click sortable' => TRUE,
    ),
    // Information for accepting a nid as an argument
    'argument' => array(
      'handler' => 'views_handler_argument_user_vid',
      'click sortable' => TRUE,
      'numeric' => TRUE,
    ),
    // Information for accepting a uid as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a uid.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'title' => t('User'),
      'label' => t('Get the actual node from a user revision.'),
    ),
  );

  // name
  $data['user_revision']['name'] = array(
    'title' => t('Name'), // The item it appears as on the UI,
    'help' => t('The title of the node.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'field' => 'name', // the real field
      // @TODO
      'handler' => 'views_handler_field_user_revision',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // log field
  $data['user_revision']['log'] = array(
    'title' => t('Log message'), // The item it appears as on the UI,
    'help' => t('The log message entered when the revision was created.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'handler' => 'views_handler_field_xss',
     ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // revision timestamp
  // changed field
  $data['user_revision']['timestamp'] = array(
    'title' => t('Created date'), // The item it appears as on the UI,
    'help' => t('The date the user revision was created.'), // The help that appears on the UI,
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['user_revision']['revert_revision'] = array(
    'field' => array(
      'title' => t('Revert link'),
      'help' => t('Provide a simple link to revert to the revision.'),
      'handler' => 'views_handler_field_user_revision_link_revert',
    ),
  );

  $data['user_revision']['delete_revision'] = array(
    'field' => array(
      'title' => t('Delete link'),
      'help' => t('Provide a simple link to delete the user revision.'),
      'handler' => 'views_handler_field_user_revision_link_delete',
    ),
  );

  return $data;
}

