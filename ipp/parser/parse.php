<?php
ini_set('display_errors', 'stderr');
$DEBUG = false;

$args = getopt("",array("help"));

if (array_key_exists("help", $args)) {
    echo("Pouziti: php7.4 parse.php < soubor.ippcode21\nCte kod v jazyce IPPcode21 z stdin, provede jeho analyzu a na stdout vypise jeho XML reprezentaci\n");
    exit(0);
}

function exit_with(int $id, string $message) {
    global $DEBUG;
    if ($DEBUG) {
        fprintf(STDERR, $message."\n");
    }
    exit($id);
}

/**
 * Třída pro šablony instrukcí 
 */
class InstructionTemplate {

    private $argc; /// počet argumentů instrukce
    private $argt; /// typy argumentů instrukce

    /**
     * @param $argt pole typů argumentů instrukce, default null
     */
    public function __construct($argt = null) {
        if ($argt != null) {
            $this->argc = count($argt);
        } else {
            $this->argc = 0;
        }
        $this->argt = $argt;
    }

    /**
     * Funkce pro ověření správnosti syntaxe instrukce a její rozložení do objektů Instruction a Argument
     * @param $instruction řetězec s celým řádkem instrukce
     */
    public function match($instruction) {
        $parts = explode(" ", $instruction, 4); // řetězec předpokládám trimovaný, opcode a argumenty odděleny jednou mezerou
        $pc = count($parts);
        if ($pc != $this->argc+1) { // +1, protože $parts obsahuje i opcode
            exit_with(23, "neočekávaný počet argumentů, očekáváno $this->argc, získáno ".strval($pc-1));
        }  
        $args = array();
        for ($i=1; $i < $pc; $i++) {
            switch($this->argt[$i-1]) { // -1, protože $parts obsahuje i opcode
                case "var":
                    if (!preg_match('/^(L|G|T)(F@)([A-Za-z_\-\$\&\%\*\!\?][A-Za-z0-9_\-\$\&\%\*\!\?]*)$/', $parts[$i])) { // kontrola formátu proměnných
                       exit_with(23, "očekáván neterminál <var> na pozici $i řádku $instruction"); // TODO exceptions
                    } else {
                        $args[] = new Argument("var", $parts[$i]);
                    }
                break;
                case "label":
                    if (!preg_match('/^([A-Za-z0-9_\-\$\&\%\*\!\?]+)$/', $parts[$i])) { // kontrola validního labelu
                        exit_with(23, "očekáván neterminál <label> na pozici $i řádku $instruction"); 
                    } else {
                        $args[] = new Argument("label", $parts[$i]);
                    }
                break;
                case "type":
                    if (!preg_match('/^(int|string|bool)$/', $parts[$i])) {
                        exit_with(23, "očekáván neterminál <type> na pozici $i řádku $instruction");
                    } else {
                        $args[] = new Argument("type", $parts[$i]);
                    }
                break;
                case "symb":
                    if (!preg_match('/^(L|G|T)(F@)([A-Za-z_\-\$\&\%\*\!\?][A-Za-z0-9_\-\$\&\%\*\!\?]*)$/', $parts[$i])) {
                        if (!preg_match('/(^int@([+-])?[0-9]+$)|(^string@([^\#\s])*$)|(^nil@nil$)|(^bool@(true|false)$)/', $parts[$i])) { // kontrola validního formátu konstanty
                            exit_with(23, "očekáván neterminál <var> na pozici $i řádku $instruction");
                        } else {
                            if (preg_match('/(\\\(([0-9]{1,2}[^0-9])|([^nt\\"0-9])))|\\\$/', $parts[$i])) {
                                exit_with(23, "neznámá escapovací posloupnost");
                            }
                            $arg = explode('@',$parts[$i]);
                            if ($arg[0] == "string") {
                                $arg[1] = /*htmlspecialchars(*/implode('@',array_slice($arg,1))/*, ENT_XML1, "UTF-8", false)*/; // ošetření problematických XML znaků
                            }
                            $args[] = new Argument($arg[0],$arg[1]);
                        }
                    } else {
                        $args[] = new Argument("var", $parts[$i]);
                    }
            }
        }
        $inst = new Instruction($parts[0], $args);
        return $inst;
    }
}

/**
 * Pomocná třída pro argumenty intrukcí
 */
class Argument {
    private $type;
    private $value;

    public function __construct($type, $value) {
        $this->type = $type;
        $this->value = $value;
    }

    public function getType() {
        return $this->type;
    }

    public function getValue() {
        return $this->value;
    }
}

/**
 * Pomocná třída pro instrukci
 */
class Instruction {

    public $opcode;
    public $args;

    /// $args je pole Argument
    public function __construct($opcode, $args) {
        $this->opcode = $opcode;
        $this->args = $args;
    }

    public function getop() {
        return strtoupper($this->opcode);
    }

    public function getargs() {
        $ret = array(
            "argc" => count($this->args)
        );
        foreach($this->args as $a) {
            $ret[] = $a;
        }
        return $ret;
    }

}

/// Seznam povolených instrukcí s pomocnými objekty
$instructions = array(
    "move" => new InstructionTemplate(["var", "symb"]),
    "createframe" => new InstructionTemplate(),
    "pushframe" => new InstructionTemplate(),
    "popframe" => new InstructionTemplate(),
    "defvar" => new InstructionTemplate(["var"]),
    "call" => new InstructionTemplate(["label"]),
    "return" => new InstructionTemplate(),
    "pushs" => new InstructionTemplate(["symb"]),
    "pops" => new InstructionTemplate(["var"]),
    "add" => new InstructionTemplate(["var", "symb", "symb"]),
    "sub" => new InstructionTemplate(["var", "symb", "symb"]),
    "mul" => new InstructionTemplate(["var", "symb", "symb"]),
    "idiv" => new InstructionTemplate(["var", "symb", "symb"]),
    "lt" => new InstructionTemplate(["var", "symb", "symb"]),
    "gt" => new InstructionTemplate(["var", "symb", "symb"]),
    "eq" => new InstructionTemplate(["var", "symb", "symb"]),
    "and" => new InstructionTemplate(["var", "symb", "symb"]),
    "or" => new InstructionTemplate(["var", "symb", "symb"]),
    "not" => new InstructionTemplate(["var", "symb"]),
    "int2char" => new InstructionTemplate(["var", "symb"]),
    "stri2int" => new InstructionTemplate(["var", "symb", "symb"]),
    "read" => new InstructionTemplate(["var", "type"]),
    "write" => new InstructionTemplate(["symb"]),
    "concat" => new InstructionTemplate(["var", "symb", "symb"]),
    "strlen" => new InstructionTemplate(["var", "symb"]),
    "getchar" => new InstructionTemplate(["var", "symb", "symb"]),
    "setchar" => new InstructionTemplate(["var", "symb", "symb"]),
    "type" => new InstructionTemplate(["var", "symb"]),
    "label" => new InstructionTemplate(["label"]),
    "jump" => new InstructionTemplate(["label"]),
    "jumpifeq" => new InstructionTemplate(["label", "symb", "symb"]),
    "jumpifneq" => new InstructionTemplate(["label", "symb", "symb"]),
    "exit" => new InstructionTemplate(["symb"]),
    "dprint" => new InstructionTemplate(["symb"]),
    "break" => new InstructionTemplate()
);

$lines = array();
while ($line = fgets(STDIN)) {
    $lines[] = $line;
}

// Ořez komentářů a prázdných řádků
$i = 0;
foreach($lines as &$l) {
    $l = preg_replace("/( |\t)*#.*/", "", $l); // odstranění komentářů
    $l = preg_replace("/( +|\t+)/", " ", $l); // vyčištění více mezer a tabů
    $l = trim($l); // trim bílých znaků na konci a začátku
    if (preg_match("/^( |\t|\n)*$/", $l) == 1) {
        unset($lines[$i]);
    }
    $i++;
}
$lines = array_values($lines);

// Kontrola přítomnosti hlavičky
if (strtolower($lines[0]) == ".ippcode21") {
    unset($lines[0]);
    $lines = array_values($lines);
} else {
    exit_with(21, "chybí hlavička");
}

// Ověření instrukcí a převod na objekty
$instlist = array();
foreach ($lines as &$l) {
    $opcode = strtolower(explode(" ", $l,2)[0]);
    if (array_key_exists($opcode, $instructions)) { // existence opcodu
        $instr = $instructions[$opcode]->match($l); // syntaktická a lexikální správnost
        $instlist[] = $instr;       
    } else {
        exit_with(22, "neznámá instrukce: $opcode\n");
    }
}

// Převod na XML
$xmlbase = '<?xml version="1.0" encoding="UTF-8"?><program></program>';
$xml = new SimpleXMLElement($xmlbase);
$xml->addAttribute('language', 'IPPcode21');
$ic = 0; // počítadlo intrukcí pro parametr 'order'

foreach ($instlist as &$inst) {
    $ic++;
    $newelem = $xml->addChild('instruction');
    $newelem->addAttribute('order', $ic);
    $newelem->addAttribute('opcode', $inst->getop());
    $args = $inst->getargs();
    for ($i = 1; $i <= $args['argc']; $i++) {
        $child = "arg$i";
        $arg = $newelem->addChild($child);
        $arg->addAttribute('type',$args[$i-1]->getType()); // -1 kvůli indexovánní arg od 1 v XML
        $newelem->$child = $args[$i-1]->getValue();
    }
}

$dom = new DOMDocument();
$dom->formatOutput = true;
$dom->preserveWhiteSpace = false;
$dom->loadXML($xml->asXML());
echo($dom->saveXML());
?>