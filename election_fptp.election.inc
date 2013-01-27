<?php
/**
 * @file
 * Election hooks for the Election FPTP module.
 */

/**
 * Implements hook_election_type_info().
 */
function election_fptp_election_type_info() {
  return array(
    'fptp' => array(
      'name' => t('FPTP election'),
      'description' => t('A First Past the Post election where people can select a single candidate to vote for.'),
      'post name' => t('position'),
      'has candidates' => TRUE,
      'vote form' => 'election_fptp_vote_form',
      'export' => TRUE,
    ),
  );
}
