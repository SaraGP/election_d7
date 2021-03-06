<?php

/**
 * @file
 * Class election_post_views_handler_field_link.
 */

/**
 * Field handler to present a link to the post's main page.
 */
class election_post_views_handler_field_link extends views_handler_field {

  /**
   * Overrides parent::construct().
   */
  public function construct() {
    parent::construct();
    $this->additional_fields['post_id'] = 'post_id';
  }

  /**
   * Overrides parent::option_definition().
   */
  public function option_definition() {
    $options = parent::option_definition();

    $options['text'] = array('default' => '', 'translatable' => TRUE);

    return $options;
  }

  /**
   * Overrides parent::options_form().
   */
  public function options_form(&$form, &$form_state) {
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
    parent::options_form($form, $form_state);
  }

  /**
   * Overrides parent::query().
   */
  public function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  /**
   * Overrides parent::render().
   */
  public function render($values) {
    $value = $this->get_value($values, 'post_id');
    return $this->render_link($this->sanitize_value($value), $values);
  }

  /**
   * Overrides parent::render_link().
   */
  public function render_link($data, $values) {
    $this->options['alter']['make_link'] = TRUE;
    $this->options['alter']['path'] = election_post_uri_path((object) array('post_id' => $data));
    $text = !empty($this->options['text']) ? $this->options['text'] : t('View details');
    return $text;
  }

}
