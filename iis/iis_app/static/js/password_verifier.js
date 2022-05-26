function checkPw(elem, secondId) {
    let se = document.getElementById(secondId);
    if (elem.value != se.value) {
        elem.setCustomValidity("Passwords does not match!");
        se.setCustomValidity("Passwords does not match!");
    } else {
        elem.setCustomValidity("");
        se.setCustomValidity("");
    }
}