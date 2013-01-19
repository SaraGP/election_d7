<?php
/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Drupal manner.
 */

/**
 * Defines election types.
 *
 * Default election types are provided by the election_stv and
 * election_referendum sub-modules. Other types can be provided using this hook.
 * Information about existing election types is loaded via the election_types()
 * function.
 *
 * For working examples, see:
 *   election_types/referendum/election_referendum.election.inc
 *   election_types/stv/election_stv.election.inc
 *
 * @return array
 *   An array of election type arrays, keyed by a unique machine name. Election
 *   type arrays can include:
 *   - name: (Required) The human-readable name of the election type, sanitized.
 *   - description: A description of the election type, sanitized.
 *   - post name: The name of the election's posts, as a singular noun. See
 *     README.txt. The name for a referendum's posts may be 'question', while
 *     another election's posts may be referred to as 'role', 'position', or
 *     'title', perhaps.
 *   - post name plural: The plural version of the post name (default: post name
 *     + s).
 *   - has candidates: Whether the election type has candidates. Modules
 *     providing elections that have candidates should depend on the
 *     election_candidate submodule.
 *   - save votes: The name of a function that saves 'votes' for this election
 *     type to the {election_vote} table. This function runs inside the
 *     same database transaction that saves 'ballots' to the {election_ballot}
 *     table. To roll back the transaction it would throw an exception. The
 *     function takes the parameters:
 *         array $form        - The form array from election_vote_form.
 *         array $form_state  - The form_state array from election_vote_form.
 *         stdClass $post     - The election post object.
 *         stdClass $election - The election object.
 *         int $ballot_id     - The ID of the ballot that is being saved.
 *   - export: Whether the election results can be exported as an archive of
 *     ballot (.blt) files. This needs the election_export submodule.
 *
 * @see election_types()
 */
function hook_election_type_info() {
  return array(
    'fptp' => array(
      'name' => t('FPTP election'),
      'description' => t('A first-past-the-post election type, where voters can select a single candidate who wins based on a simple majority with a quorum.'),
      'post name' => t('position'),
      'has candidates' => TRUE,
      'export' => TRUE,
    ),
  );
}
