// var land_names = JSON.parse($("#indexContent").data("land_names"));
// var air_names = JSON.parse($("#indexContent").data("air_names"));
// var stop_names = JSON.parse($("#indexContent").data("stop_names"));
try {
    var land_names = JSON.parse(document.getElementById("indexContent").dataset.land_names);
    var air_names = JSON.parse(document.getElementById("indexContent").dataset.air_names);

} catch (error){}
try {
    var stop_names = JSON.parse(document.getElementById("indexContent").dataset.stop_names);
    var carrier_names = JSON.parse(document.getElementById("indexContent").dataset.carrier_names);
    var vehicle_names = JSON.parse(document.getElementById("indexContent").dataset.vehicle_names);


} catch (error){}

$("#from-bar").autocomplete({
    source: land_names
});
$("#to-bar").autocomplete({
    source: land_names
});
$("#from-bar2").autocomplete({
    source: air_names
});
$("#to-bar2").autocomplete({
    source: air_names
});

$("#con-from-bar").autocomplete({
    source: stop_names
});
$("#con-to-bar").autocomplete({
    source: stop_names
});
$("#carrier_input").autocomplete({
    source: carrier_names
});
$("#vehicle_input").autocomplete({
    source: vehicle_names
});