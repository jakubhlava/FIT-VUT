/**
 * Podpůrné fuknce pro vytváření balíčků
 * Vytvořil: Jakub Hlava (xhlava52)
 */

/**
 * Generuje položky s otázkami
 * @param num   čísl ootázky
 * @returns {{question: string, menuEntry: string}}
 */
function gen_question(num) {
    return {
        question: `<div class="my-2 p-4 rounded-3 shadow" style="display:none;">
             <div class="d-none justify-content-between align-items-center" id="undo${num}">
                <span class="text-muted">Odstraněná otázka</span><button class="btn btn-sm btn-outline-secondary" onclick="restoreQuestion(${num})">Obnovit</button>
            </div>
            <div id="question${num}" class="question" data-question-order="${num}">
                <div class="d-flex justify-content-between align-items-center">
                    <div class="fs-6 fw-bold question-header"></div>
                    <button class="btn btn-sm btn-outline-danger" onclick="deleteQuestion(${num})">
                        <span class="material-icons">delete_forever</span>
                    </button>
                </div>
                <div class="row my-2 mx-2">
                    <label for="q${num}" class="form-label col-12 col-lg-2">Otázka</label>
                    <input type="text" class="form-control col bg-secondary text-light qinput" id="q${num}">
                </div>
                <div class="row mx-2">
                    <label for="a${num}" class="form-label col-12 col-lg-2">Odpověď</label>
                    <textarea rows="3" class="form-control col bg-secondary text-light" id="a${num}"></textarea>
                </div>
            </div>
    </div>`,
        menuEntry: `<a href="#q${num}" class="list-group-item list-group item action sidequestion" id="qlink${num}" style="display:none;"><span class="sq-number"></span>: <span id="qshort${num}"></span></a>`
    };
}

/**
 * Zavolá generování otázky, postará se o režii okolo (přidání do menu, ...)
 */
function createQuestion() {
    let cardlist = $('#cardList');
    let sideq = $('#sideQuestions');
    let q = gen_question(question_indexer);
    $(q.question).appendTo(cardlist).show('fast');
    $(q.menuEntry).appendTo(sideq).show('fast');
    let qinput = $('#q'+question_indexer);
    let qs = $('#qshort'+question_indexer);
    qinput.on('input', (e) => {
        let t = qinput.val();
        qs.text(t);
    });
    fixNumbers();
    question_indexer++;
}

/**
 * Skryje otázkua označí ji pro smazání (nepropoíše se do DB, případně se z DB smaže, je alke z UI obnovitelná)
 * @param num   ID otázky
 */
function deleteQuestion(num) {
    $('#question'+num).hide('slow').removeClass('question').addClass('question-removed');
    $('#undo'+num).removeClass('d-none').addClass('d-flex');
    $('#qlink'+num).hide('slow');
    fixNumbers();
}

/**
 * Obnoví "smazanou otázku"
 * @param num   ID otázky
 */
function restoreQuestion(num) {
    $('#question'+num).show('fast').removeClass('question-remove').addClass('question');
    $('#undo'+num).removeClass('d-flex').addClass('d-none');
    $('#qlink'+num).show('slow');
    fixNumbers();
}

/**
 * Přečísluje čísla v uživatelově pohledu
 */
function fixNumbers() {
    $('.question').each((i, elem) => {
        $(elem).find('.question-header').html(`Otázka ${i+1}`);
    });
    $('.sidequestion').each((i, elem) => {
        $(elem).find('.sq-number').html(`${i+1}`);
    });
}

/**
 * Vybere ze všech prvků inforamce o otázkách a vytvoří z nich objekt odeslatelný na server pomocí AJAX
 * @returns {*[]}
 */
function aggregateQuestions() {
    let questions = [];
    let send = true;
    $('.question').each((i, elem) => {
        let q = $(elem).find('input').val();
        let a = $(elem).find('textarea').val().replace(/\n/g, "<br>");
        let id = parseInt($(elem).attr("data-question-order"));
        if (q === "" || a === "") {
            send = false;
            toast.fire({
                "title": "Otázky nelze uložit. " + $(elem).find(".question-header").text() + " nemá vyplněnou otázku nebo odpověď!",
                "icon": "error"
            });
        } else {
            let obj = {
                "_id": id,
                "question": q,
                "answer": a
            }
            questions.push(obj);
        }
    });
    console.log(questions);
    return {
      questions: questions,
        send: send
    };
}

/**
 * Provede uložení otázek na server pomocí zaslání objektu na AJAX endpoint
 * @param auto  pozn. zda se jedná o autmatické uložení pro formát toast notifikace
 */
function save(auto=false) {
    let agg = aggregateQuestions();
    if (agg.send) {
        $.ajax(saving_endpoint, {
        type: "POST",
        data: JSON.stringify({questions: agg.questions}),
        contentType: "application/json; charset=utf-8",
        dataType: "json",
        success: (data, status, xhr) => {
            let title = 'Balíček byl automaticky uložen!'
            if (!auto) {
                title = 'Balíček byl úspěšně uložen!'
            }
            toast.fire({
                title: title,
                icon: 'success',
            });
        },
        error: (xhr, status, error) => {
            toast.fire({
                title: 'Chyba při ukládání balíčku!',
                icon: 'error',
            });
        }
        });
    }

}

/**
 * Vytvoří dialog pro potvrzení smazání balíčku a případně provede smazání balíčku
 */
function confirmDeletion() {
    Swal.fire({
        title: "Pozor!",
        icon: "warning",
        html: "Opravdu chcete smazat balíček? Tato akce je <b class='text-danger'>nevratná</b>!",
        customClass: {
            confirmButton: "btn btn-danger mx-2",
            cancelButton: "btn btn-success mx-2"
        },
        showConfirmButton: true,
        showCancelButton: true,
        cancelButtonText: "Ne!",
        confirmButtonText: "Smazat",
        buttonsStyling: false
    }).then((result) => {
        if (result.isConfirmed) {
            $.redirect({url: deletion_endpoint, method: "GET"});
        }
    })
}

/**
 * Uloží informace o balíčku (název, popisek)
 */
function saveInfo() {
    let name = $('#pkgEditName').val();
    let desc = $('#pkgEditDesc').val();
    if (name === "") {
        Swal.fire({
            title: "Tak tohle by nešlo!",
            icon: "error",
            html: "Balíček musí mít jméno, pojmenuj ho a zkus to znovu!",
            customClass: {
                confirmButton: "btn btn-success mx-2"
            },
            showConfirmButton: true,
            confirmButtonText: "Omlouvám se, jdu ho pojmenovat!",
            buttonsStyling: false
        });
    } else {
        $.ajax(edit_info_endpoint, {
        method: "POST",
        data: {
            name: name,
            desc: desc
        },
        success: (data, status, xhr) => {
            toast.fire({
                title: 'Informace byly uloženy!',
                icon: 'success',
            });
        },
        error: (xhr, status, error) => {
            toast.fire({
                title: 'Chyba při ukládání informací!',
                icon: 'error',
            });
        }
    });
    }

}

// autosave každých 30s
setInterval(() => {
    save(true);
}, 30000);


$(document).ready(() => {
    $('.qinput').each((i, element) => {
       let elem = $(element);
       let id = elem.attr('id').substring(1);
       let qs = $('#qshort'+id);
       elem.on('input', (e) => {
           let t = elem.val();
           qs.text(t);
       });
    });
});