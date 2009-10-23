<?php
// $Id$

/**
 * @file
 * Admin page callbacks for the uc_ticket module.
 */

function uc_ticket_list_tickets() {

  if (empty($_SESSION['uc_ticket_list_filter'])) {
    $_SESSION['uc_ticket_list_filter']['status'][1] = variable_get('uc_ticket_default_filter', 1);
  }

  $filter = uc_ticket_build_filter_query();

  $output = drupal_get_form('uc_ticket_filter_form');
  $sort = variable_get('uc_ticket_default_sort', 'desc');
  $SQL = "SELECT * FROM {uc_ticket_tickets}";
  $limit = 20;
  $header = array(
    array('data' => t('ID'), 'field' => 'ticket_id'),
    array('data' => t('Subject')),
    array('data' => t('Status'), 'field' => 'status'),
    array('data' => t('Category'), 'field' => 'category'),
    array('data' => t('Created'), 'field' => 'created'),
    array('data' => t('Updated'), 'field' => 'updated', 'sort' => $sort),
    array('data' => t('Functions')),
  );
  $tablesort = tablesort_sql($header);

  if (!empty($filter['where'])) {
    $result = pager_query($SQL ." WHERE ". $filter['where'] . $tablesort, $limit, 0, NULL, $filter['args']);
  }
  else {
    $result = pager_query($SQL . $tablesort, $limit, 0, NULL);
  }
	
  $rows = array();
  while ($data = db_fetch_object($result)) {
    $row = array();
    $row[] = array('data' => $data->ticket_id, 'class' => 'username-cell');
    $row[] = array('data' => l($data->title, 'admin/store/ticket/' . $data->ticket_id), 'class' => 'title-cell');
    $row[] = array('data' => uc_ticket_get_status_name($data->status), 'class' => 'username-cell');
    $row[] = array('data' => uc_ticket_get_category_name($data->category), 'class' => 'username-cell');
    $row[] = array('data' => date('m/d/Y h:i A', $data->created), 'class' => 'date-cell');
    $row[] = array('data' => date('m/d/Y h:i A', $data->updated), 'class' => 'date-cell');
    $links = l(t('VIEW'), 'admin/store/ticket/' . $data->ticket_id);
    $links .= (user_access('delete tickets')) ? ' | ' . l(t('DELETE'), 'admin/store/ticket/' . $data->ticket_id . '/delete') : '';
    $row[] = array('data' => $links, 'class' => 'username-cell');
    $rows[] = $row;
  }    
  if (!$rows) {
    $rows[] = array(array('data' => t('No results'), 'colspan' => 7));
  }
	
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, $limit, 0);

  return $output;

}

function uc_ticket_search_form(&$form_state) {

  $form = array();

  $form['ticket_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ticket search'),
  );
	
  $form['ticket_search']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Search tickets by email address'),
    '#size' => 64,
    '#description' => t(""),
    '#required' => FALSE,
    '#default_value' => (arg(5) != '0') ? arg(5) : '',
  );

  $form['ticket_search']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Search tickets by title'),
    '#size' => 64,
    '#description' => t(""),
    '#required' => FALSE,
    '#default_value' => (arg(6) != '0') ? arg(6) : '',
  );
	
  $form['ticket_search']['submit'] = array('#type' => 'submit', '#value' => t('Search'));
	
  //results table
  if (arg(4) == 'results') {
    $email = drupal_strtolower(str_replace(' ', '%', arg(5)));
    $title = drupal_strtolower(str_replace(' ', '%', arg(6)));
    $SQL = "SELECT * FROM {uc_ticket_tickets} WHERE (LOWER(title) LIKE '%%%s%' OR LOWER(email) LIKE '%s')";
    $limit = 20;
    $header = array(
    array('data' => t('ID'), 'field' => 'ticket_id'),
    array('data' => t('Subject')),
    array('data' => t('Status'), 'field' => 'status'),
    array('data' => t('Category'), 'field' => 'category'),
    array('data' => t('Created'), 'field' => 'created'),
    array('data' => t('Updated'), 'field' => 'updated', 'sort' => 'asc'),
    array('data' => t('Functions')),
  );
  $tablesort = tablesort_sql($header);
  $result = pager_query($SQL . $tablesort, $limit, 0, NULL, $title, $email);
  $rows = array();
  while ($data = db_fetch_object($result)) {
    $row = array();
    $row[] = array('data' => $data->ticket_id, 'class' => 'username-cell');
    $row[] = array('data' => l($data->title, 'admin/store/ticket/' . $data->ticket_id), 'class' => 'title-cell');
    $row[] = array('data' => uc_ticket_get_status_name($data->status), 'class' => 'username-cell');
    $row[] = array('data' => uc_ticket_get_category_name($data->category), 'class' => 'username-cell');
    $row[] = array('data' => date('m/d/Y h:i A', $data->created), 'class' => 'date-cell');
    $row[] = array('data' => date('m/d/Y h:i A', $data->updated), 'class' => 'date-cell');
    $links = l(t('VIEW'), 'admin/store/ticket/' . $data->ticket_id);
    $links .= (user_access('delete tickets')) ? ' | ' . l(t('DELETE'), 'admin/store/ticket/' . $data->ticket_id . '/delete') : '';
    $row[] = array('data' => $links, 'class' => 'username-cell');
    $rows[] = $row;
  }
  if (!$rows) {
    $rows[] = array(array('data' => t('No results found that match your search query. Please try again with different keywords.'), 'colspan' => 7));
  }
  $output = theme('table', $header, $rows);
  $output .= theme('pager', NULL, $limit, 0);
  $form['results'] = array('#value' => $output);
  }
	
  return $form;

}

function uc_ticket_search_form_submit($form, &$form_state) {

  $email = ($form_state['values']['email']) ? $form_state['values']['email'] : 0;
  $title = ($form_state['values']['title']) ? $form_state['values']['title'] : 0;

  $form_state['redirect'] = 'admin/store/ticket/search/results/'. $email . '/' . $title;

}

function uc_ticket_create_ticket_form(&$form_state) {

  $form = array();

  $order_id = arg(4);

  $SQL = "SELECT primary_email FROM {uc_orders} WHERE order_id = %d";
  $email = db_result(db_query($SQL, $order_id));

  $form['description'] = array(
    '#value' => t('Use this form to create a new ticket. There is a link on each View Order page that will prepopulate the Order ID and Customer Email fields.'),
  );

  $form['new_ticket'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create a new ticket'),
  );

  $form['new_ticket']['order_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Order ID'),
    '#default_value' => ($order_id) ? $order_id : '',
    '#size' => 10,
    '#maxlength' => 10,
    '#required' => FALSE,
  );

  $form['new_ticket']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Customer email'),
    '#default_value' => ($email) ? $email : '',
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
  );

  $statuses = uc_ticket_get_statuses();

  $form['new_ticket']['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => $statuses,
  );

  $categories = uc_ticket_get_categories();

  $form['new_ticket']['category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => $categories,
  );

  $templates = uc_ticket_get_templates();
  $templates[0] = 'Please select...';
  ksort($templates);

  $form['new_ticket']['template'] = array(
    '#type' => 'select',
    '#title' => t('Template'),
    '#default_value' => $templates[0],
    '#options' => $templates, 
  );

  $form['new_ticket']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => '',
    '#size' => 80,
    '#maxlength' => 120,
    '#required' => TRUE,
  );

  $form['new_ticket']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#required' => TRUE,
    '#rows' => 15,
  );

  $form['new_ticket']['sig'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add signature to ticket?'),
  );

  $form['new_ticket']['submit'] = array('#type' => 'submit', '#value' => t('Submit new ticket'));

  $form['#validate'] = array('uc_ticket_create_validate');

  return $form;

}

function uc_ticket_create_validate($form, &$form_state) {

  if ($form_state['values']['order_id'] != '') {
    $SQL = "SELECT created FROM {uc_orders} WHERE order_id = %d";
    $exists = db_result(db_query($SQL, $form_state['values']['order_id']));
    if (!$exists) {
      form_set_error('order_id', t('That order can not be found.'));
    }
  }

}

function uc_ticket_create_ticket_form_submit($form, &$form_state) {

  global $user;
  $time = time();
  $email = $form_state['values']['email'];
  $order_id = $form_state['values']['order_id'];
  $status = $form_state['values']['status'];
  $body = $form_state['values']['body'];
  if ($form_state['values']['sig'] == TRUE) {  
    $signature = uc_ticket_get_signature($user->uid);
    $body .= "\r\n\r\n" . $signature;
  }
  $category_id = $form_state['values']['category'];
  $category_name = uc_ticket_get_category_name($form_state['values']['category']);
  $subject = $form_state['values']['subject'];
  //get uid from order
  $SQL = "SELECT uid FROM {uc_orders} WHERE order_id = %d";
  $uid = db_result(db_query($SQL, $order_id));
  //create ticket
  $SQL = "INSERT INTO {uc_ticket_tickets} (created, uid, email, order_id, status, assigned_to, updated, title, category) VALUES (%d, %d, '%s', %d, %d, %d, %d, '%s', %d)";
  db_query($SQL, $time, $uid, $email, $order_id, $status, 0, $time, $subject, $category_id);
  $ticket_id = db_last_insert_id('uc_ticket_tickets', 'ticket_id');
  //create followup data
  $SQL = "INSERT INTO {uc_ticket_tickets_data} (ticket_id, delta, created, email, subject, body) VALUES (%d, 0, %d, '%s', '%s', '%s')";
  db_query($SQL, $ticket_id, $time, $user->name, $subject, $body);
  $mail_subject = $subject . ' [T#' . $ticket_id . ']';
  if ($ticket_trail->order_id) {
    $mail_subject .= '[O#' . $ticket_trail->order_id . ']';
  }
  uc_ticket_email_customer($ticket_id, $mail_subject, $body);
  drupal_set_message(t('Ticket added.'));
  $form_state['redirect'] = 'admin/store/ticket/' . $ticket_id;

}

function uc_ticket_delete_ticket_form(&$form_state, $ticket_id) {

  $form = array();

  $form['description'] = array(
    '#value' => '<span>' . t('Are you sure you want to delete this ticket?') . '</span>',
  );
  $form['yes'] = array('#type' => 'submit', '#value' => t('Yes'));
  $form['no'] = array('#type' => 'submit', '#value' => t('No'));
  $form['ticket_id'] = array('#type' => 'hidden', '#value' => $ticket_id);

  return $form;

}

function uc_ticket_delete_ticket_form_submit($form, &$form_state) {

  $ticket_id = $form_state['values']['ticket_id'];
  if ($form_state['values']['op'] == t('Yes')) {
    $SQL = "DELETE {uc_ticket_tickets}, {uc_ticket_tickets_data} FROM {uc_ticket_tickets}, {uc_ticket_tickets_data} WHERE uc_ticket_tickets.ticket_id = %d AND uc_ticket_tickets_data.ticket_id = %d";
    db_query($SQL, $ticket_id, $ticket_id);
    drupal_set_message(t('Ticket @ticket_id deleted.', array('@ticket_id' => $ticket_id)));
    $form_state['redirect'] = 'admin/store/ticket';
  } else {
    $form_state['redirect'] = 'admin/store/ticket/' . $ticket_id;
  }

}

function uc_ticket_display_ticket($ticket_id) {

  //load ticket trail
  $ticket_trail = uc_ticket_load_ticket_trail($ticket_id);

  $output = '';
  $output = drupal_get_form('uc_ticket_view_controls_form', $ticket_trail);

  //ticket info box 
  $output .= '<div id="info_box"><fieldset><legend>' . t('Ticket Information') . '</legend>';
  $output .= '<strong>' . t('Ticket ID:') . '</strong> ' . $ticket_trail->ticket_id . '<br />';
  $output .= '<strong>' . t('Created on:') . '</strong> ' . date('m/d/Y g:i A', $ticket_trail->created) . '<br />';
  $output .= '<strong>' . t('Last update:') . '</strong> ' . date('m/d/Y g:i A', $ticket_trail->updated) . '<br />';
  $output .= '<strong>' . t('Category:') . '</strong> ' . uc_ticket_get_category_name($ticket_trail->category) . '<br />';
  $output .= '<strong>' . t('Status:') . '</strong> ' . uc_ticket_get_status_name($ticket_trail->status) . '<br />';
  $output .= '<strong>' . t('Order:') . '</strong> ' . $order_id = ($ticket_trail->order_id) ? l($ticket_trail->order_id, 'admin/store/orders/' . $ticket_trail->order_id) : t('none') . '<br />';
  $output .= '</fieldset>';

  //user info box
  $output .= '<fieldset><legend>' . t('User Information') . '</legend>';
  if ($ticket_trail->uid != 0) {
    $user = user_load($ticket_trail->uid);
    $SQL = "SELECT SUM(order_total) as total, count(*) as count, MAX(created) as last FROM {uc_orders} WHERE uid = %d";
    $result = db_query($SQL, $user->uid);
    $order_data = db_fetch_object($result);
    $output .= '<strong>' . t('Username:') . '</strong> ' . $user->name . '<br />';
    $output .= '<strong>' . t('Email:') . '</strong> ' . $user->mail . '<br />';
    if ($order_data->count) {
      $output .= '<strong>' . t('Purchases:') . '</strong> ' . l($order_data->count, 'admin/store/customers/orders/' . $user->uid) . '<br />';
      $output .= '<strong>' . t('Order total:') . '</strong> ' . uc_currency_format($order_data->total) . '<br />';
      $output .= '<strong>' . t('Last purchase:') . '</strong> ' . date('m/d/Y g:i A', $order_data->last) . '<br />';
    }   
  } else {
    $output .= t('None available. Please add user information to ticket.');
  }
  $output .= '</fieldset></div>';

  $count = count($ticket_trail->data);
  $i = $count;
  foreach ($ticket_trail->data as $data) {
    $collapsed = ($i != $count) ? ' collapsed' : '';
    $box = '<div class="ticket_box"><fieldset class="collapsible'. $collapsed . '"><legend>(' . ($data->delta+1) . ') - ' . date('m/d/Y g:i A', $data->created) . '</legend>';
    $box .= '<h3 class="ticket_title">' . $data->subject . '</h3>';
    $box .= '<span class="ticket_date">' . date('m/d/Y g:i A', $data->created) . ' by ' . $data->email . '</span>';
    $box .= '<p>' . check_markup($data->body) . '</p>';
    $box .= '</fieldset></div>';
    $output .= $box;
    $i--;
  }

  return $output;

} 

function uc_ticket_view_controls_form(&$form_state, $ticket) {

  $form = array();

  $form['controls'] = array(
    '#type' => 'fieldset',
    '#title' => t('Ticket controls'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $statuses = uc_ticket_get_statuses();

  $form['controls']['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#default_value' => $ticket->status,
    '#options' => $statuses,
  );

  $categories = uc_ticket_get_categories();

  $form['controls']['category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#default_value' => $ticket->category,
    '#options' => $categories,
  );

  $form['controls']['order_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Order ID'),
    '#size' => 12,
    '#description' => t(""),
    '#required' => FALSE,
    '#default_value' => ($ticket->order_id) ? $ticket->order_id : '',
  );

  $output = '<div id="form_functions">';
  if (user_access('delete tickets')) {
    $output .= l('<img src="/' . drupal_get_path('module', 'uc_ticket') . '/images/delete.png' . '" alt="' . t('Delete Ticket') . '" />', 'admin/store/ticket/' . $ticket->ticket_id . '/delete', array('html' => TRUE));
  }
  $output .= l('<img src="/' . drupal_get_path('module', 'uc_ticket') . '/images/add.png' . '" alt="' . t('Add Followup To Ticket') . '" />', 'admin/store/ticket/followup/' . $ticket->ticket_id, array('html' => TRUE));
  $output .= '</div>';

  $form['controls']['functions'] = array(
    '#value' => $output,
  );

  $form['ticket_id'] = array('#type' => 'hidden', '#value' => $ticket->ticket_id);
  $form['#validate'] = array('uc_ticket_view_controls_form_validate');
  $form['controls']['submit'] = array('#type' => 'submit', '#value' => t('Update ticket'));

  return $form;

}

function uc_ticket_view_controls_form_validate($form, &$form_state) {

  if ($form_state['values']['order_id'] != '' && $form_state['values']['order_id'] != 0) {
    $SQL = "SELECT created FROM {uc_orders} WHERE order_id = %d";
    $exists = db_result(db_query($SQL, $form_state['values']['order_id']));
    if (!$exists) {
      form_set_error('order_id', t('That order can not be found.'));
    }
  }
  
}

function uc_ticket_view_controls_form_submit($form, &$form_state) {

  $ticket_id = $form_state['values']['ticket_id'];
  $status = $form_state['values']['status'];
  $category = $form_state['values']['category'];
  $order_id = $form_state['values']['order_id'];
  //fetch uid from order
  $SQL = "SELECT uid FROM {uc_orders} WHERE order_id = %d";
  $uid = db_result(db_query($SQL, $order_id));

  $SQL = "UPDATE {uc_ticket_tickets} SET status = %d, category = %d, order_id = %d, uid = %d WHERE ticket_id = %d";
  db_query($SQL, $status, $category, $order_id, $uid, $ticket_id);
  drupal_set_message(t('Changes saved.'));

}

function uc_ticket_create_followup_form(&$form_state, $ticket_id) {

  $form = array();

  $SQL = "SELECT * FROM {uc_ticket_tickets} WHERE ticket_id = %d";
  $ticket = db_fetch_object(db_query($SQL, $ticket_id));

  $form['description'] = array(
    '#value' => 'Use this form to create a followup to a ticket.',
  );

  $form['return'] = array(
    '#value' => '<div id="return_link">' . l(t('Go back to ticket @ticket_id.', array('@ticket_id' => $ticket_id)), 'admin/store/ticket/' . $ticket_id) . '</div>',
  );

  $form['followup'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create a followup to ticket ' . $ticket_id),
  );

  $form['followup']['email'] = array(
    '#type' => 'textfield',
    '#title' => t('Customer email'),
    '#default_value' => $ticket->email,
    '#size' => 40,
    '#maxlength' => 40,
    '#required' => TRUE,
  );

  $statuses = uc_ticket_get_statuses();

  $form['followup']['status'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#default_value' => $ticket->status,
    '#options' => $statuses,
  );

  $templates = uc_ticket_get_templates();
  $templates[0] = 'Please Select...';
  ksort($templates);

  $form['followup']['template'] = array(
    '#type' => 'select',
    '#title' => t('Template'),
    '#default_value' => $templates[0],
    '#options' => $templates, 
  );

  $form['followup']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#default_value' => $ticket->title,
    '#size' => 80,
    '#maxlength' => 120,
    '#required' => TRUE,
  );

  $form['followup']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Body'),
    '#required' => TRUE,
    '#rows' => 15,
  );

  $form['followup']['send'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send email to customer?'),
  );

  $form['followup']['sig'] = array(
    '#type' => 'checkbox',
    '#title' => t('Add signature to followup?'),
  );

  $form['followup']['ticket_id'] = array(
    '#type' => 'hidden',
    '#value' => $ticket_id,
  );

  $form['followup']['submit'] = array('#type' => 'submit', '#value' => t('Submit followup'));

  return $form;

}

function uc_ticket_create_followup_form_submit($form, &$form_state) {

  global $user;
  $time = time();
  $status = $form_state['values']['status'];
  $ticket_id = $form_state['values']['ticket_id'];
  $ticket_trail = uc_ticket_load_ticket_trail($ticket_id);
  $email = $form_state['values']['email'];
  $body = $form_state['values']['body'];
  if ($form_state['values']['sig'] == TRUE) {  
    $signature = uc_ticket_get_signature($user->uid);
    $body .= "\r\n\r\n" . $signature;
  }
  $clean_subject = $form_state['values']['subject'];
  $subject = $form_state['values']['subject'] . ' [T#' . $ticket_id . ']';
  if ($ticket_trail->order_id) {
    $subject .= '[O#' . $ticket_trail->order_id . ']';
  }
  $delta = count($ticket_trail->data);
  //create followup data
  $SQL = "INSERT INTO {uc_ticket_tickets_data} (ticket_id, delta, created, email, subject, body) VALUES (%d, %d, %d, '%s', '%s', '%s')";
  db_query($SQL, $ticket_id, $delta, $time, $user->name, $clean_subject, $body);
  //update ticket updated field
  $SQL = "UPDATE {uc_ticket_tickets} SET updated = %d, status = %d WHERE ticket_id = %d";
  db_query($SQL, $time, $status, $ticket_id);
  if ($form_state['values']['send'] == TRUE) {
    uc_ticket_email_customer($ticket_id, $subject, $body);
    drupal_set_message('Email sent to customer.');
  }
  drupal_set_message(t('Followup to ticket @ticket_id added.', array('@ticket_id' => $ticket_id)));
  $form_state['redirect'] = 'admin/store/ticket/' . $ticket_id;
}


function uc_ticket_admin_settings_form() {

  $form = array();

  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );

  $form['settings']['filter'] = array(
    '#type' => 'select',
    '#title' => t('Ticket list filter'),
    '#default_value' => variable_get('uc_ticket_default_filter', 1),
    '#options' => uc_ticket_get_statuses(),
    '#description' => t('Initial filter for the ticket list (Status)'),
  );

  $form['settings']['sort'] = array(
    '#type' => 'select',
    '#title' => t('Ticket list sort'),
    '#default_value' => variable_get('uc_ticket_default_sort', 'desc'),
    '#options' => array(
      'desc' => 'Newest updates first',
      'asc'  => 'Oldest updates first',
    ),
    '#description' => t('Initial sort for the ticket list (Updated date)'),
  );

  $form['followup']['submit'] = array('#type' => 'submit', '#value' => t('Save settings'));

  return $form;

}

function uc_ticket_admin_settings_form_submit($form, &$form_state) {

  $filter = $form_state['values']['filter'];
  $sort = $form_state['values']['sort'];
  variable_set('uc_ticket_default_filter', $filter);
  variable_set('uc_ticket_default_sort', $sort);

  drupal_set_message(t('Settings saved.'));

}

function uc_ticket_admin_email_accounts_view_form() {

  $form = array();

  $SQL = "SELECT email_id, protocol, server, username FROM {uc_ticket_email}";
  $limit = 10;
  $header = array(
    array('data' => t('ID'), 'field' => 'email_id', 'sort' => 'asc'),
    array('data' => t('Protocol'), 'field' => 'protocol'),
    array('data' => t('Server name'), 'field' => 'server'),
    array('data' => t('Username'), 'field' => 'username'),
    array('data' => t('Functions')),
  );
  $tablesort = tablesort_sql($header);
  $result = pager_query($SQL . $tablesort, $limit, 0, NULL);
  $rows = array();
  while ($data = db_fetch_object($result)) {
    $row = array();
    $row[] = array('data' => $data->email_id, 'class' => 'id-cell');
    $row[] = array('data' => $data->protocol, 'class' => 'proto-cell');
    $row[] = array('data' => $data->server, 'class' => 'server-cell');
    $row[] = array('data' => $data->username, 'class' => 'username-cell');
    $row[] = array('data' => l(t('EDIT'), 'node/edit') . ' | ' . l(t('DELETE'), 'node'), 'class' => 'function-cell');
    $rows[] = $row;
  }    

  if (!$rows) {
    $rows[] = array(array('data' => t('There are no email accounts set up. ') . l(t('You can create them here'), 'admin/store/settings/ticket/edit/email/add') . '.', 'colspan' => 5));
  }

  $output = theme('table', $header, $rows);
  $output .= theme('pager', NULL, $limit, 0);
  $form['email'] = array('#value' => $output);

  return $form;

}

function uc_ticket_admin_email_accounts_add_form(&$form_state) {

  $form = array();

  $form['description'] = array(
    '#value' => 'Please fill in all of the required fields to add a new email account.',
  );

  $form['protocol'] = array(
    '#type' => 'select',
    '#title' => t('Email Server Protocol'),
    '#default_value' => $form_state['storage']['protocol'],
    '#options' => array(
      'pop3' => t('POP3'),
      'imap' => t('IMAP'),
    ),
    '#description' => t('The protocol in use for this email account. Typically this is POP3.'),
  );

  $form['server'] = array(
    '#type' => 'textfield',
    '#title' => t('Server Address'),
    '#default_value' => $form_state['storage']['server'],
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('Enter the address you would use to fetch email for this account. ex. mail.example.com'),
  );

  $form['username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => $form_state['storage']['username'],
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('Enter the username for this account'),
  );

  $form['password'] = array(
    '#type' => 'password',
    '#title' => t('Password'),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('Enter the password for the account.'),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Save'));
  $form['test'] = array('#type' => 'submit', '#value' => t('Test connection'));
  $form['fetch'] = array('#type' => 'submit', '#value' => t('Fetch mail'));

  $form['#validate'] = array('uc_ticket_admin_email_accounts_add_form_validate');

  return $form;

}

function uc_ticket_admin_email_accounts_add_form_validate($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Save')) {
    $SQL = "SELECT email_id FROM {uc_ticket_email} WHERE protocol='%s' AND server='%s' AND username='%s'";
    $exists = db_result(db_query($SQL, $form_state['values']['protocol'], $form_state['values']['server'], $form_state['values']['username']));
    if ($exists) {
      form_set_error('username', t('That account already exists in the database.'));
    }
  }
  
}

function uc_ticket_admin_email_accounts_add_form_submit($form, &$form_state) {

  if ($form_state['clicked_button']['#value'] == t('Save')) {
    $SQL = "INSERT INTO {uc_ticket_email} (protocol, server, username, password) VALUES ('%s', '%s', '%s', '%s')";
    db_query($SQL, $form_state['values']['protocol'], $form_state['values']['server'], $form_state['values']['username'], $form_state['values']['password']);
    drupal_set_message(t('Account added sucessfully.'));
    unset($form_state['storage']);
  } else if ($form_state['clicked_button']['#value'] == t('Test Connection')) {
    if ($form_state['values']['protocol'] == 'pop3') {    
      $conn = imap_open('{' . $form_state['values']['server'] . ':110/' . $form_state['values']['protocol'] . '/novalidate-cert}INBOX', $form_state['values']['username'], $form_state['values']['password']);
      //drupal_set_message('<pre>' . print_r(imap_errors(), TRUE) . '</pre>');
    }
    if ($conn) {
      drupal_set_message(t('Connection tested successfully.'));
    }
    $form_state['storage'] = $form_state['values'];
    $form_state['rebuild'] = TRUE; 
  } else if ($form_state['clicked_button']['#value'] == t('Fetch Mail')) {
    //get email accounts and fetch mail
    $SQL = "SELECT * FROM {uc_ticket_email}";
    $result = db_query($SQL);
    while ($account = db_fetch_object($result)) {
      _uc_ticket_fetch_mail($account);
    }
  }

}

/************
**
** The following four functions...
**
**    uc_ticket_build_filter_query
**    uc_ticket_filters
**    uc_ticket_filter_form
**    uc_ticket_filter_form_submit
**
**  ...were borrowed from the dblog module. Used for sorting
**  the ticket list.
**
************/

function uc_ticket_build_filter_query() {
  
  if (empty($_SESSION['uc_ticket_list_filter'])) {
    return;
  }

  $filters = uc_ticket_filters();

  // Build query
  $where = $args = array();
  foreach ($_SESSION['uc_ticket_list_filter'] as $key => $filter) {
    $filter_where = array();
    foreach ($filter as $value) {
      $filter_where[] = $filters[$key]['where'];
      $args[] = $value;
    }
    if (!empty($filter_where)) {
      $where[] = '('. implode(' OR ', $filter_where) .')';
    }
  }
  $where = !empty($where) ? implode(' AND ', $where) : '';

  return array(
    'where' => $where,
    'args' => $args,
  );
}

function uc_ticket_filters() {
  
  $filters = array();

  $filters['status'] = array(
    'title' => t('Status'),
    'where' => "status = %d",
    'options' => uc_ticket_get_statuses(),
  );

  $filters['category'] = array(
    'title' => t('Category'),
    'where' => 'category = %d',
    'options' => uc_ticket_get_categories(),
  );

  return $filters;
}

function uc_ticket_filter_form() {
  $session = &$_SESSION['uc_ticket_list_filter'];
  $session = is_array($session) ? $session : array();
  $filters = uc_ticket_filters();

  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter by status or category'),
    '#collapsible' => TRUE,
    '#collapsed' => empty($session),
  );
  foreach ($filters as $key => $filter) {
    $form['filters']['status'][$key] = array(
      '#title' => $filter['title'],
      '#type' => 'select',
      '#multiple' => TRUE,
      '#options' => $filter['options'],
    );
    if (!empty($session[$key])) {
      $form['filters']['status'][$key]['#default_value'] = $session[$key];
    } else {
      
    }
  }

  $form['filters']['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  if (!empty($session)) {
    $form['filters']['buttons']['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Reset')
    );
  }

  return $form;
}

function uc_ticket_filter_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  $filters = uc_ticket_filters();
  switch ($op) {
    case t('Filter'):
      foreach ($filters as $name => $filter) {
        if (isset($form_state['values'][$name])) {
          $_SESSION['uc_ticket_list_filter'][$name] = $form_state['values'][$name];
        }
      }
      break;
    case t('Reset'):
      $_SESSION['uc_ticket_list_filter'] = array();
      break;
  }
  return 'admin/store/ticket';
}

function uc_ticket_template_form(&$form_state) {

  $form = array();

  $templates = uc_ticket_get_templates();

  $form['template'] = array(
    '#type' => 'fieldset',
    '#title' => t('Templates'),
    '#collapsible' => FALSE,
    '#theme' => 'uc_ticket_templates_table',
    '#tree' => TRUE,
  );

  foreach ($templates as $id => $name) {
    $form['template'][$id]['id'] = array(
      '#value' => $id,
    );

    $form['template'][$id]['name'] = array(
      '#value' => $name,
    );

    $form['template'][$id]['remove'] = array(
      '#type' => 'checkbox',
    );
    
    $form['template'][$id]['edit'] = array(
      '#value' => l(t('EDIT'), 'admin/store/settings/ticket/template/edit/' . $id),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit changes'),
  );

  $form['create'] = array(
    '#type' => 'submit',
    '#value' => t('Create new template'),
  );

  return $form;

}

function uc_ticket_template_form_submit($form, &$form_state) {

  switch ($form_state['values']['op']) {
    case t('Submit changes'):
      foreach ($form_state['values']['template'] as $key => $value) {
        if ($value['remove'] == TRUE) {
          db_query("DELETE FROM {uc_ticket_templates} WHERE template_id = %d", $key);
          drupal_set_message(t('Template ID %id removed.', array('%id' => $key)));
        }
      }
    break;

    case t('Create new template'):
      $form_state['redirect'] = 'admin/store/settings/ticket/template/add';
    break;
  }

}

function uc_ticket_template_edit_form(&$form_state, $template_id) {

  $form = array();

  $SQL = "SELECT * FROM {uc_ticket_templates} WHERE template_id = %d";
  $result = db_query($SQL, $template_id);

  $template = db_fetch_object($result);

  $form['return'] = array(
    '#value' => '<div id="return_link">' . l(t('Go back to template overview.'), 'admin/store/settings/ticket/template') . '</div>',
  );

  $form['edit'] = array(
    '#type' => 'fieldset',
    '#title' => t('Edit template (ID# ' . $template->template_id . ')'),
    '#collapsible' => FALSE,
    '#description' => t('Use this form to edit a template.'),
  );

  $form['edit']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Template name'),
    '#default_value' => $template->template_name,
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('The name of the template'),
  );

  $form['edit']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Template subject'),
    '#default_value' => $template->template_subject,
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('The text that will appear in the subject line.'),
  );

  $form['edit']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Template body'),
    '#required' => TRUE,
    '#rows' => 15,
    '#default_value' => $template->template_text,
    '#description' => t('The text that will appear in the body of the ticket.'),
  );

  $form['edit']['template_id'] = array('#type' => 'hidden', '#value' => $template->template_id);

  $form['edit']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit changes'),
  );

  return $form;

}

function uc_ticket_template_edit_form_submit($form, &$form_state) {

  $name = $form_state['values']['name'];
  $subject = $form_state['values']['subject'];
  $text = $form_state['values']['body'];
  $id = $form_state['values']['template_id'];
  $SQL = "UPDATE {uc_ticket_templates} SET template_name = '%s', template_subject = '%s', template_text = '%s' WHERE template_id = %d";
  db_query($SQL, $name, $subject, $text, $id);
  drupal_set_message(t('Changes saved.'));
  $form_state['redirect'] = 'admin/store/settings/ticket/template';

}

function uc_ticket_template_add_form(&$form_state) {

  $form = array();

  $form['return'] = array(
    '#value' => '<div id="return_link">' . l(t('Go back to template overview.'), 'admin/store/settings/ticket/template') . '</div>',
  );

  $form['add'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a new template'),
    '#collapsible' => FALSE,
    '#description' => t('Use this form to add a new template.'),
  );

  $form['add']['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Template name'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('The name of the template'),
  );

  $form['add']['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Template subject'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => TRUE,
    '#description' => t('The text that will appear in the subject line.'),
  );

  $form['add']['body'] = array(
    '#type' => 'textarea',
    '#title' => t('Template body'),
    '#required' => TRUE,
    '#rows' => 15,
    '#default_value' => '',
    '#description' => t('The text that will appear in the body of the ticket.'),
  );

  $form['add']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;


}

function uc_ticket_template_add_form_submit($form, &$form_state) {

  $name = $form_state['values']['name'];
  $subject = $form_state['values']['subject'];
  $text = $form_state['values']['body'];
  $SQL = "INSERT INTO {uc_ticket_templates} (template_name, template_subject, template_text) VALUES ('%s', '%s', '%s')";
  db_query($SQL, $name, $subject, $text);
  drupal_set_message(t('Template created.'));
  $form_state['redirect'] = 'admin/store/settings/ticket/template';

}

function uc_ticket_status_form(&$form_state) {

  $form = array();

  $statuses = uc_ticket_get_statuses();

  $form['statuses'] = array(
    '#type' => 'fieldset',
    '#title' => t('Statuses'),
    '#collapsible' => FALSE,
    '#theme' => 'uc_ticket_statuses_table',
    '#tree' => TRUE,
  );

  foreach ($statuses as $id => $name) {
    $form['statuses'][$id]['id'] = array(
      '#value' => $id,
    );

    $form['statuses'][$id]['status'] = array(
      '#value' => $name,
    );

    $form['statuses'][$id]['remove'] = array(
      '#type' => 'checkbox',
    ); 
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit changes'),
  );

  $form['add'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add a new status'),
    '#collapsible' => FALSE,
  );

  $form['add']['new'] = array(
    '#type' => 'textfield',
    '#title' => t('New status name'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#description' => t('The text you enter here will appear anywhere this status is shown.'),
  );

  $form['add']['create'] = array(
    '#type' => 'submit',
    '#value' => t('Create new status'),
  );

  return $form;

}

function uc_ticket_status_form_submit($form, &$form_state) {

  switch ($form_state['values']['op']) {
    case t('Submit changes'):
      foreach ($form_state['values']['statuses'] as $key => $value) {
        if ($value['remove'] == TRUE) {
          db_query("DELETE FROM {uc_ticket_status} WHERE status_id = %d", $key);
          drupal_set_message(t('Status ID %id removed.', array('%id' => $key)));
        }
      }
    break;

    case t('Create new status'):
      if (!$form_state['values']['new']) {
        drupal_set_message(t('You must enter a name to create a new status.'));
      } else {
        $name = $form_state['values']['new'];
        $SQL = "INSERT INTO {uc_ticket_status} (status_name) VALUES ('%s')";
        db_query($SQL, $name);
        drupal_set_message(t('Status %name created.', array('%name' => $name)));
      }
    break;
  }

}

function uc_ticket_signature_form(&$form_state) {

  global $user;  

  $signature = uc_ticket_get_signature($user->uid);

  $form = array();

  $form['sig'] = array(
    '#type' => 'fieldset',
    '#title' => t('Your signature'),
    '#collapsible' => FALSE,
  );

  $has_sig = ($signature) ? TRUE : FALSE;
  $form['sig']['has_sig'] = array('#type' => 'hidden', '#value' => $has_sig);

  $form['sig']['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Signature'),
    '#required' => TRUE,
    '#rows' => 10,
    '#default_value' => $signature,
    '#description' => t('This will appear on any new tickets or followups to tickets when you check the Add Signature checkbox.'),
  );

  $form['sig']['user'] = array('#type' => 'hidden', '#value' => $user->uid);

  $form['sig']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save signature'),
  );

  return $form;

}

function uc_ticket_signature_form_submit($form, &$form_state) {

  $uid = $form_state['values']['user'];
  $signature = $form_state['values']['text'];

  if ($form_state['values']['has_sig']) {
    $SQL = "UPDATE {uc_ticket_signatures} SET text = '%s' WHERE uid = %d";
    db_query($SQL, $signature, $uid);
    drupal_set_message(t('Signature updated.'));
  } else {
    $SQL = "INSERT INTO {uc_ticket_signatures} (uid, text) VALUES (%d, '%s')";
    db_query($SQL, $uid, $signature);
    drupal_set_message(t('Signature added.'));
  }
  
}
