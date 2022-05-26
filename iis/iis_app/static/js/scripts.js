function swap(id1, id2)
{
    var tmpValue = document.getElementById(id1).value;
    document.getElementById(id1).value = document.getElementById(id2).value;
    document.getElementById(id2).value = tmpValue;
}