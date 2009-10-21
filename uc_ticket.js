// $Id$

$(document).ready(function(){
  $('#edit-template').change(function(){
    var basepath = Drupal.settings.basePath;
    var tid = $('#edit-template').val();
    $.getJSON(basepath+"uc_ticket_fetch_template/"+tid, function(j){
      if (j.text != false) {
        $('#edit-body').val(j.text);
        $('#edit-subject').val(j.subject);
      }
    });
  });
});
