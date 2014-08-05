<?php

/**
 * Build the email form that customers will use to send the emails.
 */
function worx_social_review_email_form() {
  $form['email_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Email Subject'),
    '#default_value' => variable_get('email_subject', ''),
  );
  $form['email_from'] = array(
    '#type' => 'textfield',
    '#title' => t('Email From Address'),
    '#default_value' => variable_get('email_from', ''),
  );
  $form['email_to'] = array(
    '#type' => 'textfield',
    '#title' => t('Email To Address'),
  );
  $form['tokens'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tokens'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  $output = array();
  $string = array();
  if (variable_get('worx_social_review_google_link', '')) {
    $string[] = '@google';
    $output['@google'] = '[google] - Google Review Link';
  }
  if (variable_get('worx_social_review_bing_link', '')) {
    $string[] = '@bing';
    $output['@bing'] = '[bing] - Bing Review Link';
  }
  if (variable_get('worx_social_review_yahoo_link', '')) {
    $string[] = '@yahoo';
    $output['@yahoo'] = '[yahoo] - Yahoo Review Link';
  }
  if (variable_get('worx_social_review_yp_link', '')) {
    $string[] = '@yp';
    $output['@yp'] = '[yp] - Yellow Pages Review Link';
  }
  if (variable_get('worx_social_review_foursquare_link', '')) {
    $string[] = '@foursquare';
    $output['@foursquare'] = '[foursquare] - Foursquare Review Link';
  }
  if (variable_get('worx_social_review_urban_spoon_link', '')) {
    $string[] = '@urbanspoon';
    $output['@urbanspoon'] = '[urbanspoon] - Urban Spoon Review Link';
  }
  if (variable_get('worx_social_review_ta_link', '')) {
    $string[] = '@ta';
    $output['@ta'] = '[ta] - Trip Advisor Review Link';
  }
  $form['tokens']['token_list'] = array(
    '#type' => 'item',
    '#markup' => '<p>' . t(implode('<br />', $string), $output ) . '</p>',
  );

  $body = variable_get('email_body', '');
  $form['email_body'] = array(
    '#type' => 'text_format',
    '#title' => t('Email Body'),
    '#default_value' => $body['value'],
    '#format' => 'filtered_html',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('submit'),
  );

  return $form;
}

/*
 * Define the function worx_social_review_email_form_submit.
 */
function worx_social_review_email_form_submit($form, $form_state) {
  $params = array();
  $params['body'] = $form_state['values']['email_body']['value'];
  $from = $form_state['values']['email_from'];
  $to = $form_state['values']['email_to'];
  $body = str_replace(
    $search = array(
      '[google]',
      '[yahoo]',
      '[bing]',
      '[yp]',
      '[foursquare]',
      '[urbanspoon]',
      '[ta]',
    ),
    $replace = array(
      variable_get('worx_social_review_google_link', 'Error: No link. Please contact site administrator.'),
      variable_get('worx_social_review_yahoo_link', 'Error: No link. Please contact site administrator.'),
      variable_get('worx_social_review_bing_link', 'Error: No link. Please contact site administrator.'),
      variable_get('worx_social_review_yp_link', 'Error: No link. Please contact site administrator.'),
      variable_get('worx_social_review_foursquare_link', 'Error: No link. Please contact site administrator.'),
      variable_get('worx_social_review_urban_spoon_link', 'Error: No link. Please contact site administrator.'),
      variable_get('worx_social_review_ta_link', 'Error: No link. Please contact site administrator.'),
    ),
    $subject = $params['body']
  );
  $subject = $form_state['values']['email_subject'];
  $message = array(
    'key' => 'worx_social_review',
    'to' => $to,
    'from' => $from,
    'subject' => $subject,
    'body' => $body,
    'headers' => array(
      'From' => $from,
      'Sender' => $from,
      'Return-Path' => $from,
    ),
  );
  $dupe = $message;
  $message = mimemail_prepare_message($message);
  mimemail_mailengine('send', $message);
  $dupe['to'] = 'reviews@worxco.net';
  $dupe = mimemail_prepare_message($dupe);
  mimemail_mailengine('send', $dupe);
  drupal_set_message('Email successfully sent to ' . $form_state['values']['email_to']);
}
