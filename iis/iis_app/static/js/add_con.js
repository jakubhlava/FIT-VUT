document.getElementById('first-stop').textContent = document.getElementById('con-from-bar').value
$('#con-from-bar').on('change', function () {
  document.getElementById('first-stop').textContent = document.getElementById('con-from-bar').value
})

document.getElementById('first-stop-time').textContent = document.getElementById('departure-time-bar').value
$('#departure-time-bar').on('change', function () {
  document.getElementById('first-stop-time').textContent = document.getElementById('departure-time-bar').value
})

document.getElementById('last-stop').textContent = document.getElementById('con-to-bar').value
$('#con-to-bar').on('change', function () {
  document.getElementById('last-stop').textContent = document.getElementById('con-to-bar').value
})

document.getElementById('last-stop-time').textContent = document.getElementById('arrival-time-bar').value
$('#arrival-time-bar').on('change', function () {
  document.getElementById('last-stop-time').textContent = document.getElementById('arrival-time-bar').value
})

function startloading() {
  let connbut = $('#addconn')
  $('#editformcon').validate({
    submitHandler: (form) => {
      connbut.attr('disabled', true);
      connbut.text(" Adding connection...");
      form.submit();
    }
  });
  return true;
}

function addrow() {

  var html_var = `
    </div>
    <div id="`+ "ID_row" + nmb + `" class="row mb-2 g-3 align-items-center">
      <div class="col-sm-2">
        <label for="edit_role" class="col-form-label" style="font-size: 1.7vh"></label>
      </div>
      <div class="col">
        <div class="form-floating">
          <input id="`+ "ID_time" + nmb + `" name="` + "ID_time" + nmb + `" type="time" class="form-control" form="editformcon" required>
          <label for="time-bar">Time:</label>
          <div class="invalid-feedback">
              Please fill time of the next stop!
          </div>
        </div>
      </div>
      <div class="col">
        <div class="form-floating">
          <input id="`+ "ID_stop" + nmb + `" name="` + "ID_stop" + nmb + `" type="search" class="form-control" form="editformcon" required>
          <label for="con-to-bar">Name:</label>
          <div class="invalid-feedback">
              Please fill name of the next stop!
          </div>
        </div>
        
      </div>

      <div class="col">
      <div class="form-floating">
          <input id="`+ "stop_price" + nmb + `" type="number" min="0" oninput="this.value = Math.round(this.value);" class="form-control"
              name="`+ "stop_price" + nmb + `" required>
          <label for="`+ "stop_price" + nmb + `">Price:</label>
          <div class="invalid-feedback">
          Value must be greater then or equal to 0!
          </div>
        
      </div>
  </div>
      
      <div class="col">
      <button class="btn btn-outline-danger mx-2 my-2 my-sm-0 pl-4" onclick="delrow(` + nmb + `)"><i class="bi bi-x-lg"></i></button>
      </div>



    </div>
  </div>
  `

  $("#stop_list").append(html_var)

  for (let i = 0; i <= nmb; i++) {
    $('#ID_stop' + i).autocomplete({
      source: stop_names
    });
  }
  nmb = nmb + 1

}

function delrow(row) {


  $("#ID_row" + row).remove()


}