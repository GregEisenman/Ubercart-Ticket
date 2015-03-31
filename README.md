Ubercart Ticket is a ticketing/customer support system for Ubercart.
## Features:

*   **Email integration** - Ubercart Ticket can be configured to collect emails from any number of email addresses and creates tickets from the content of the email
*   **Integration with Ubercart orders** - an order pane is created on every order that lists all tickets for that order along with a link to create a new ticket with that order's information
*   **Integration with the Contact module** - Ubercart Ticket adds an 'order picker' to the contact form so that customers can choose which order their question applies to and links that order with the ticket
*   **Customer information on tickets** - Useful customer information is displayed on each ticket. This includes order information, customer contact information, total orders placed, total dollar amount spent, etc.
*   **Templates** - customer service reps can create templates for responses to save time when answering emails
*   **Outgoing email attachments** - customer service reps can attach documents (PDF, Word etc.) related to the ticket to the outgoing email to the customer.

## Installation:

1.  Upload the uc_ticket directory to /sites/all/modules
2.  Install the module by navigating to admin/build/modules and activating the module. You can find it under Ubercart Extra. You may be prompted to enable the Contact module. Do so.
3.  Navigate to admin/build/contact/add and add a few categories. For each, under recipients, make sure the email you enter is one that you will check with this module. You will set that account up in uc_ticket next.
4.  Navigate to admin/store/settings/ticket/email/add to set up an email account. Select POP3 or IMAP, add port, add any required flags, add the server address, the username and the password for the email account you would like uc_ticket to check. Test the connection, then Save.
5.  Navigate to admin/store/settings/ticket/status. Here you can add statuses for tickets. Typically this would include 'Open', 'Closed' and maybe a few others.
6.  Drupal will check for new emails with every cron run, so set cron to run at an appropriate time. This is beyond the scope of these instructions. Please read[this](http://drupal.org/cron) if you need help configuring cron.
7.  That's it! You can add templates at admin/store/settings/ticket/template and your signature at admin/store/settings/ticket/signature if you like.

* Your PHP installation_must_ have the[IMAP extension](http://www.php.net/manual/en/book.imap.php) installed for inbound mail functionality to work.
