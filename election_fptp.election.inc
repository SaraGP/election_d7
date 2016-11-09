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
      'description' => t('A First Past the Post election: voters select a single choice, and the candidate with the most votes wins.'),
      'has candidates' => TRUE,
      'vote form' => 'election_fptp_vote_form',
      'export' => TRUE,
    ),
  );
}
