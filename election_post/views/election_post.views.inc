<?php
/**
 * @file
 * Views integration for the Election module.
 */

/**
 * Implements hook_views_data().
 */
function election_views_data() {

  $data = array();

  // ----------------------------------------------------------------
  // election posts table -- basic table information.

  // Define the base group of this table. Fields that don't
  // have a group defined will go into this field by default.
  $data['election_post']['table']['group']  = t('Election post');

  // Advertise this table as a possible base table
  $data['election_post']['table']['base'] = array(
    'field' => 'candidate_id',
    'title' => t('Election post'),
    'entity type' => 'election_post',
    'weight' => -9,
//    'access query tag' => 'election_access', // @todo work out how to adapt this
    'defaults' => array(
      'field' => 'post_id',
    ),
  );

  // Join posts to elections
  $data['election']['table']['join']['election_post'] = array(
    'left_field' => 'election_id',
    'field' => 'election_id',
  );

  // post_id
  $data['election_post']['post_id'] = array(
    'title' => t('Post ID'),
    'help' => t('The unique ID of the election post.'), // The help that appears on the UI,
    // Information for displaying the post_id
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    // Information for accepting a post_id as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    // Information for sorting on a post_id.
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // title
  $data['election_post']['title'] = array(
    'title' => t('Title'), // The item it appears as on the UI,
    'help' => t('The title of the election post.'), // The help that appears on the UI,
     // Information for displaying a title as a field
    'field' => array(
      'handler' => 'views_handler_field_election_post',
      'click sortable' => TRUE,
      'link_to_election default' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    // Information for accepting a title as a filter
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;

}