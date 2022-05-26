

function Addrow_updatelabel(val, id, text) {
  document.getElementById(id).innerHTML = text + val;
  $('.vsc').remove();


  for (let i = 1; i <= val; i++) {
    var html_var = `
        <div id="ID_row${i}" class="row mb-2 g-3 align-items-center vsc">
        <div class="col-sm-2">
          <label for="edit_role" class="col-form-label">Section:</label>
        </div>
        <div class="col">
          <div class="form-floating" >
              <input id="sect_name${i}" name="sect_name${i}" type="text" class="form-control sname" required>
                <label for="sect_name${i}">Section name:</label>
                <div class="invalid-feedback">
                    Name or describe this section
                    </div>
          </div>
        </div>
        <div class="col">
          <div>
            <label for="seat_nmb${i}" class="form-label" id="seat_nmb_label${i}" >Number of seats:</label>
            <input type="number" class="form-control scount" value="1" min="1" id="seat_nmb${i}" name="seats_num${i}" required>
            <div class="invalid-feedback">
              Number of seats must be greater then or equal 1.
              </div>
          </div>
        </div>
        </div>
        `

      $("#list-of-sections").append(html_var)
  }
}


