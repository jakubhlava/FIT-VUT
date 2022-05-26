<?php
/**
 * Autor: Jakub Hlava (xhlava52)
 */

$args = getopt("",array("help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only", "jexamxml:", "jexamcfg:"));

function print_usage() {
    echo "Použití: test.php [params]\n";
    echo "Platné parametry:\n";
    echo "\t--help                    vypíše tuto nápovědu\n";
    echo "\t--directory=<cesta>       adresář s testy (při neuvedení aktuání složka)\n";
    echo "\t--recursive               bude hledat testy i v podsložkách\n";
    echo "\t--parse-script=<soubor>   cesta ke skriptu pro analýzu zdrojového kódu (výchozí ./parse.php)\n";
    echo "\t--int-script=<soubor>     cesta ke skripti pro interpretaci XML reprezentace kódu (výchozí ./interpret.py)\n";
    echo "\t--parse-only              testuje jen skript pro analýzu, vylučuje --int-script a --int-parse\n";
    echo "\t--int-only                testuje pouze skript pro interpretaci XML reprezentace, vylučuje --parse-only a --parse-script\n";
    echo "\t--jexamxml=<soubor>       cesta k JAR balíčku A7Soft JExamXML, (výchozí /pub/courses/ipp/jexamxml/jexamxml.jar)\n";
    echo "\t--jexamcfg=<soubor>       cesta ke konfiguračnímu souboru nástroje A7Soft JExamXML (výchozí /pub/courses/ipp/jexamxml/options)\n";
}

// prohledá složku a detekuje testy
function look_for_tests(string $dir) {
    $filenames = scandir($dir);
    $files = array();
    $dirs = array();
    foreach ($filenames as $f) { // vytřídění složek a souborů - primárně pro rekurzivní prohledání 
        $fullpath = join(DIRECTORY_SEPARATOR, array($dir,$f));
        if (is_dir($fullpath)) {
            if ($f != '.' && $f != '..') {
                $dirs[] = join(DIRECTORY_SEPARATOR, array($dir,$f));
            }
        } else {
            $files[] = join(DIRECTORY_SEPARATOR, array($dir,$f));
        }
    }
    $tests = array();
    foreach ($files as $f) { // rozřazení souborů k jednotlivým testům
        $info = pathinfo($f);
        $base_path = join(DIRECTORY_SEPARATOR, array($info['dirname'],$info['filename']));
        if (in_array($info['extension'], array("rc", "in", "out", "src"))) {
            if (!array_key_exists($base_path, $tests)) {
                $tests[$base_path] = [$info['extension']];
            } else {
                $tests[$base_path][] = $info['extension'];
            }
        }
    }
    return array(
        'subdirs' => $dirs,
        'tests' => $tests
    );
}

// rekurzivní volání look_for_tests, dokud neprohledám všechny podsložky
function scan_tests_recursively(array &$dirs, array &$tests) { 
    $dir = array_pop($dirs);
    $result = look_for_tests($dir);
    $dirs = array_merge($dirs,$result['subdirs']);
    $tests = array_merge($tests,$result['tests']);
    if (count($dirs) > 0) {
        scan_tests_recursively($dirs, $tests);
    } 
}

// dotvoří případné chybějící .rc, .out a .in soubory
function prepare_files_for_tests(array $tests) {
    $paths = array_keys($tests);
    foreach ($paths as $p) {
        if (!in_array('src', $tests[$p])) {
            unset($tests[$p]);
        } else {
            if (!in_array('rc', $tests[$p])) {
                try {
                    $rcfile = fopen($p.'.rc', 'w');
                    fwrite($rcfile, "0");
                    $tests[$p][] = 'rc';
                    fclose($rcfile);
                } catch (Exception $e) {
                    fwrite(STDERR, "chyba 11: $p.rc neexistuje a nelze vytvořit: $e\n");
                    exit(11);
                }
            }
            if (!in_array('in', $tests[$p])) {
                try {
                    $rcfile = fopen($p.'.in', 'w');
                    $tests[$p][] = 'in';
                    fclose($rcfile);
                } catch (Exception $e) {
                    fwrite(STDERR, "chyba 11: $p.in neexistuje a nelze vytvořit: $e\n");
                    exit(11);
                }
            }
            if (!in_array('out', $tests[$p])) {
                try {
                    $rcfile = fopen($p.'.out', 'w');
                    $tests[$p][] = 'out';
                    fclose($rcfile);
                } catch (Exception $e) {
                    fwrite(STDERR, "chyba 11: $p.out neexistuje a nelze vytvořit: $e\n");
                    exit(11);
                }
            }
        }
    }
}

// vygeneruje strom testů (pro generování HTML)
function gen_tree($tests, $results, $reasons) {
    $tree = array("/type" => array("cat")); // speciální záznamy začínající / používám pro doplňující informace
    $stacks = array();
    $passed = 0;
    $failed = 0;
    global $tests_dir;
    $prefix = $tests_dir.DIRECTORY_SEPARATOR;
    foreach($tests as $t) { // začazení testů do stromové struktury
        $path = str_replace($prefix, '', $t);
        $parts = explode(DIRECTORY_SEPARATOR, $path);
        $pstack = array_reverse($parts); // obrácená cesta, aby se daly části cesty odebírat z konce pole
        $result = $results[$t]; // výsledek testu
        $reason = ""; // důvod chyby
        if ($result) {
            $passed++;
        } else {
            $failed++;
            $reason = $reasons[$t];
        }
        $last = &$tree; // reference na nejbližší "podkořen"
        while(count($pstack) > 0) {
            $sub = array_pop($pstack);
            if (!array_key_exists($sub, $last)) {
                $last[$sub] = array("/type" => array("cat"));
            }
            if (count($pstack) == 0) {
                $last[$sub]["/type"] = array("test");
                $last[$sub]["result"] = $result;
                if (!$result) {
                    $last[$sub]["reason"] = $reason;
                }
            } else {
                $last = &$last[$sub];
            }
        }
    }
    return array(
        "tree" => $tree,
        "passed" => $passed,
        "failed" => $failed
    );
}

// zjišťuje, zda je prvek test nebo kategorie
function is_category($subtree) {
    return $subtree["/type"][0] == "cat";
}

// vygeneruje HTML pro danou kategorii (rekurzivně)
function echo_category($name, $subtree) {
    echo('<div class="tests-category">');
    echo("<button id=\"sh-$name\" onClick=\"showHide('$name')\">+</button>");
    echo("<span>$name</span>");
    echo("<div class=\"categ-container hidden\" id=\"$name\">");
    foreach(array_keys($subtree) as $sub) {
        if ($sub == "/type") {
            continue;
        }
        if (is_category($subtree[$sub])) {
            echo_category($sub, $subtree[$sub]);
        } else {
            if ($subtree[$sub]["result"]) {
                echo("<div class=\"test passed\">$sub - PASSED</div>");
            } else {
                $reas = $subtree[$sub]["reason"];
                echo("<div class=\"test failed\">$sub - FAILED - $reas</div>");
            }
            
        }
    }
    echo('</div>');
    echo('</div>');
}

// Generuje hlavičku stránky a spouští generování kategorií
function gen_page($tests, $results, $reasons) {
    $stats = gen_tree($tests, $results, $reasons);
    $header = <<<EOD
        <!DOCTYPE html>
        <html lang="cz">
        <head>
            <meta charset="UTF-8">
            <meta http-equiv="X-UA-Compatible" content="IE=edge">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Výsledek automatických testů</title>
            <style>
                body {
                    background-color: #ababab;
                    font-family: verdana;
                } 
                div {
                    background-color: #333333;
                    color: #DDDDDD;
                    padding: 10px;
                }
                @media screen and (min-width: 900px) {
                    div.topbar {
                        display: flex;
                    }
                }
                @media screen and (max-width: 900px) {
                    div.topbar {
                        display: block;
                    }
                }
                div.topbar {
                    margin: auto;
                    margin-bottom: 20px;
                    font-size: 32pt;
                }
                @media screen and (max-width: 900px) {
                    div.topbar span {
                        display: block;
                    }
                }
                div.topbar span {
                    text-align: center;
                    flex-grow: 1;
                }
                div.topbar span#passed {
                    color: #00BA00;
                    order: 1;
                }
                div.topbar span#failed {
                    color: #BA0000;
                    order: 2;
                }
                div.topbar span#total {
                    color: #0c8ce0;
                    order: 3;
                }
                div.test {
                    padding: 3px;
                }
                div.test.failed {
                    background-color: #90000063;
                }
                div.test.passed {
                    background-color: #00900063;
                }
                div.tests-category {
                    padding: 5px;
                }
                div.tests-category span {
                    padding-left: 20px;
                }
                div.tests-category span.stat {
                    color: #AAAAAA;
                    font-size: 10pt;
                    padding-left: 10px;
                }
                div.tests-category button {
                    background-color: #DDDDDD;
                    color: #111111;
                    border: none;
                    transition-duration: 0.3s;
                    width: 20px;
                }
                div.tests-category button:hover {
                    background-color: #888;
                }
                :focus {
                    outline: none;
                }
                .hidden {
                    display: none;
                }
                .shown {
                    display: block;
                }
            </style>
            <script>
                function showHide(elem) {
                    var e = document.getElementById(elem);
                    var b = document.getElementById("sh-"+elem);
                    if (e.classList.contains("hidden")) {
                        e.classList.remove("hidden");
                        e.classList.add("shown");
                        b.innerText = "-";
                    } else {
                        e.classList.add("hidden");
                        e.classList.remove("shown");
                        b.innerText = "+";
                    }
                }
                function countStats() {
                    var cats = document.getElementsByClassName("tests-category");
                    for (cat of cats) {
                        var passed = cat.getElementsByClassName("passed").length;
                        var failed = cat.getElementsByClassName("failed").length;
                        var infoelem = document.createElement("span");
                        infoelem.className = "stat";
                        infoelem.innerText = "(passed: "+passed+", failed: "+failed+")";
                        cat.insertBefore(infoelem, cat.getElementsByClassName("categ-container")[0]);
                    }
                }
            </script>
        </head>
        <body onload="countStats()">
    EOD;
    $pcount = $stats["passed"];
    $fcount = $stats["failed"];
    $tcount = $pcount + $fcount;
    $topbar = <<<EOD
    <div class="topbar">
        <span id="passed">PASSED:&nbsp$pcount</span>
        <span id="failed">FAILED:&nbsp$fcount</span>
        <span id="total">TOTAL:&nbsp$tcount</span>
    </div>
    EOD;
    echo($header);
    echo($topbar);
    echo('<div class="tests-container">');
    $cats = array();
    $solos = array();
    foreach (array_keys($stats["tree"]) as $name) { // roztřídění na kategorie a "solo" testy v top úrovni
        if ($name == "/type") {
            continue;
        }
        if ($stats["tree"][$name]["/type"][0] == "cat") {
            $cats[$name] = $stats["tree"][$name];
        } else if ($stats["tree"][$name]["/type"][0] == "test") {
            $solos[$name] = $stats["tree"][$name];
        }
    }
    ksort($cats);
    ksort($solos);
    
    foreach (array_keys($cats) as $name) {
        echo_category($name, $stats["tree"][$name]);
    }
    foreach (array_keys($solos) as $name) {
        
        if ($stats["tree"][$name]["result"]) {
            echo("<div class=\"test passed\">$name - PASSED</div>");
        } else {
            $reas = $stats["tree"][$name]["reason"];
            echo("<div class=\"test failed\">$name - FAILED - $reas</div>");
        }
    }
    $footer = "</div></body></html>";
    echo($footer);
}

// help
if (count($args) == 0) {
    print_usage();
    exit(0);
}

if (array_key_exists("help", $args)) {
    if (count($args) > 1) {
        fprintf(STDERR, "chyba 10: kombinace --help s jiným parametrem\n");
        exit(10);
    }
    print_usage();
    exit(0);
}

// defaultní nastavení
$tests_dir = ".";
$recursive = false;
$parseonly = false;
$intonly = false;
$parser_path = "./parse.php";
$int_path = "./interpret.py";
$jexam = "/pub/courses/ipp/jexamxml/jexamxml.jar";
$jconf = "/pub/courses/ipp/jexamxml/options";

// zpracování argumentů
if (array_key_exists("directory", $args)) {
    $tests_dir = $args["directory"];
    if (!is_dir($tests_dir)) {
        fprintf(STDERR, "chyba 41: neexistující složka: $tests_dir\n");
        exit(41);
    }
}

if (array_key_exists("recursive", $args)) {
    $recursive = true;
}

if (array_key_exists("parse-script", $args)) {
    $parser_path = $args["parse-script"];
    if (!file_exists($parser_path)) {
        fprintf(STDERR, "chyba 41: neexistující soubor: $parser_path\n");
        exit(41);
    }
}

if (array_key_exists("int-script", $args)) {
    $int_path = $args["int-script"];
    if (!file_exists($int_path)) {
        fprintf(STDERR, "chyba 41: neexistující soubor: $int_path\n");
        exit(41);
    }
}

if (array_key_exists("parse-only", $args)) {
    if (array_key_exists("int-only", $args) || array_key_exists("int-script", $args)) {
        fprintf(STDERR, "chyba 10: zakázaná kombinace parametrů --parse-only a --int-only nebo --int-script\n");
        exit(10);
    }
    $parseonly = true;
}

if (array_key_exists("int-only" ,$args)) {
    if (array_key_exists("parse-only", $args) || array_key_exists("parse-script", $args)) {
        fprintf(STDERR, "chyba 10: zakázaná kombinace parametrů --int-only a --parse-only nebo --parse-script\n");
        exit(10);
    }
    $intonly = true;
}

if (array_key_exists("jexamxml", $args)) {
    $jexam = $args["jexamxml"];
    if (!file_exists($jexam)) {
        fprintf(STDERR, "chyba 41: neexistující soubor: $jexam\n");
        exit(41);
    }
}

if (array_key_exists("jexamcfg", $args)) {
    $jconf = $args["jexamcfg"];
    if (!file_exists($jconf)) {
        fprintf(STDERR, "chyba 41: neexistující soubor: $jconf\n");
        exit(41);
    }
}

$scandirs = [$tests_dir];
$tests = array();

// vyhledání testů
if ($recursive) {
    scan_tests_recursively($scandirs, $tests);
} else {
    $tests = look_for_tests($tests_dir)['tests'];
}

// doegenrování chybějících souborů
prepare_files_for_tests($tests);

$passed = array();
$failed = array();
$results = array();
$reasons = array();

// testování
if ($parseonly) {
    foreach (array_keys($tests) as $t) {
        $expected_rc = intval(file_get_contents($t.".rc"));
        $rc;
        $out;
        if ($expected_rc == 0) {
            exec("php7.4 $parser_path < $t.src > ./output.xml 2>./errors.tmp", $out, $rc); // spuštění parseru
            if ($rc == 0) {
                exec("java -jar $jexam ./output.xml $t.out ./delta.xml $jconf", $out, $rc); // porovnání výsledku
                if ($rc == 0) {
                    $results[$t] = true;
                } else { 
                    $error = file_get_contents("./delta.xml");
                    $results[$t] = false;
                    $reasons[$t] = "výstupy se neshodují: $error";
        }
            } else { 
                $results[$t] = false;
                $reasons[$t] = "špatný exit-code, očekáván 0, vrácen $rc";
            }      
        } else { // pro nenulový očekávaný exitcode nemusíme porovnávat výstup
            exec("php7.4 $parser_path < $t.src > ./output.xml 2>./errors.tmp", $out, $rc);
            if ($rc == $expected_rc) {
                $results[$t] = true;
            } else {
                $results[$t] = false;
                $reasons[$t] = "špatný exit-code, očekáván $expected_rc, vrácen $rc";
            }
        }
        if (is_file("./output.xml")) {
            unlink("./output.xml");
        }
    } 
} else if ($intonly) {
    foreach (array_keys($tests) as $t) {
        $expected_rc = intval(file_get_contents($t.".rc"));
        $rc;
        $out;
        if ($expected_rc == 0) {
            exec("python3.8 $int_path --source=$t.src --input=$t.in >./output.tmp 2>./errors.tmp", $out, $rc);
            if ($rc == 0) {
                $out = null;
                exec("diff -u $t.out ./output.tmp", $out, $rc);
                if ($rc == 0) {
                    $results[$t] = true;
                } else {
                    $results[$t] = false;
                    $reasons[$t] =  "výstupy se neshodují:\n".implode("\n",$out);
                }
            } else {
                $results[$t] = false;
                $reasons[$t] = "špatný návratový kód, očekáván $expected_rc, vrácen $rc";
            }
        } else {
            exec("python3.8 $int_path --source=$t.src --input=$t.in 2>./errors.tmp", $out, $rc);
            if ($rc == $expected_rc) {
                $results[$t] = true;
            } else {
                $results[$t] = false;
                $reasons[$t] = "špatný návratový kód, očekáván $expected_rc, vrácen $rc";
            }
        }
        if (is_file("./output.tmp")) {
            unlink("./output.tmp");
        }
        if (is_file("./errors.tmp")) {
            unlink("./errors.tmp");
        }
    }
} else {
    foreach (array_keys($tests) as $t) {
        $expected_rc = intval(file_get_contents($t.".rc"));
        $rc;
        $out;
        exec("php7.4 $parser_path < $t.src > ./output.xml 2>./errors.tmp", $out, $rc); 
        if ((($expected_rc > 23 || $expected_rc == 0) && $rc != 0) && $rc != $expected_rc) { // kontrola návratového kódu parseru, odfiltrování kód interpretu
            $results[$t] = false;
            if ($expected_rc < 30) {
                $reasons[$t] = "neodpovídá návratový kód parseru, očekávaný: $expected_rc, návratový kód $rc";
            } else {
                $reasons[$t] = "neodpovídá návratový kód parseru, očekávaný 0, návratový kód $rc";
            }  
        } else {
            exec("python3.8 $int_path --source=./output.xml --input=$t.in >./output.tmp 2>./errors.tmp", $out, $rc);
            if ($rc == 0 && $expected_rc == 0) {
                $out = null;
                exec("diff -u $t.out ./output.tmp", $out, $rc);
                if ($rc == 0) {
                    $results[$t] = true;
                } else {
                    $results[$t] = false;
                    $reasons[$t] = "výstupy se neshodují:\n".implode("\n",$out);
                }
            } else if ($rc == $expected_rc) {
                $results[$t] = true;
            } else {
                $results[$t] = false;
                $reasons[$t] = "špatný návratový kód, očekáván $expected_rc, vrácen $rc";
            }
        }
        if (is_file("./output.xml")) {
            unlink("./output.xml");
        }
        if (is_file("./output.tmp")) {
            unlink("./output.tmp");
        }
        if (is_file("./errors.tmp")) {
            unlink("./errors.tmp");
        }
    }
}

    gen_page(array_keys($tests), $results, $reasons); // vygenerování výsledku testů
?>