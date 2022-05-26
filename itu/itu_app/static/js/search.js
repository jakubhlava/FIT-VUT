/**
 * Podpůrné funkce pro vyhledávač balíčků
 * Vytvořil: Jakub Hlava (xhlava52)
 */

/**
 * Generátor nalezenéhoi záznamu
 * @param name          jméno nalezeného balíčku
 * @param description   popisek nalezenéhoi balíčku
 * @param find_code     vyhledávací kód balíčku
 * @returns {string}    HTML kód záznamu
 */
function gen_result(name, description, find_code, fav) {
    let f = `fav('${find_code}', ${!fav})`;
    let icon = "favorite_border";
    if (fav) {
        icon = "favorite";
    }
    return `
        <div class="row">
        <div class="col-12 d-flex justify-content-between"><h5>${name}</h5><span class="text-muted">${find_code}</span></div>
            
        </div>
        <div class="row">
            <p class="text-muted col-12">${description}</p>
        </div>
        <div class="d-flex flex-row justify-content-end">
            <a href="${start_base}${find_code}" class="btn btn-sm btn-outline-success ms-2">Spustit</a>
            <button class="btn btn-sm btn-outline-info ms-2" onclick="selectCategory('${find_code}')">Přidat do kategorie</button>
            <button class="btn btn-sm btn-outline-danger d-inline-flex align-items-center dropdown ms-2" id="fav-${find_code}" onclick="${f}"><span class="material-icons">${icon}</span></button>
        </div>
    `
}

let total_pages = 0;        // aktuální počet stránek záznamů
let current_page = 1;       // aktuální stránka

/**
 * Pomocí AJAX satáhne ze serveru výsledky vyhledávání a vygeneruje prvky s výsledky
 * @param page  stránka výsledků
 */
function search(page=1) {
    $.ajax(search_endpoint, {
        data: {
            query: $('#searchBar').val(),
            page: page
        },
        success: (data, status, xhr) => {
            let sr = $('#searchResults');
            sr.html("");
            data.results.forEach((elem, i) => {
               sr.append(gen_result(elem.name, elem.description, elem.find_code, elem.is_fav));
               if (i !== 9) {
                   sr.append('<hr>');
               }
            });
            total_pages = data.total;
            current_page = data.page;
            if (data.results.length === 0) {
                sr.append(`<div class="row"><h4>Pro tento dotaz nejsou žádné výsledky :(</h4></div>`)
            }
            refresh_pagination();
            console.log(data);
        }
    });
}

/**
 * Podle aktuálního stavu vyhledávače generuje nabídku pro posun mezi stránkami
 * @returns {string}
 */
function gen_pagination() {
    let prev = '';
    let next = '';
    let pages = '';
    if (current_page > 1) {
        prev = `<li class="page-item"><button class="page-link" onclick="search(${current_page-1})">Předchozí</button></li>`;
    }
    if (current_page < total_pages) {
        console.log(current_page, total_pages)
        next = `<li class="page-item"><button class="page-link" onclick="search(${current_page+1})">Následující</button></li>`;
    }
    let start = -2;
    let end = 2;
    if (current_page < 3) {
        end += 3 - current_page;
    } else if (current_page > total_pages-3) {
        start -= 3-(total_pages-current_page);
    }
    for (let i = start; i<=end; i++) {
        if (current_page+i >= 1 && current_page+i <= total_pages) {
            pages += `<li class="page-item"><button class="page-link" onclick="search(${current_page+i})">${current_page+i}</button></li>`;
        }
    }
    let base = `
    <nav>
      <ul class="pagination">
        ${prev}
        ${pages}
        ${next}        
      </ul>
    </nav>`
    return base;
}

/**
 * Volá se pro překreslení menu s výběrem stránky záznamů
 */
function refresh_pagination() {
    if (total_pages > 1) {
        $('#pagination').html(gen_pagination());
    } else {
        $('#pagination').html("");
    }
}

/**
 * Generuje položku sleect elementu pro přidávání balíčků do kategorií
 * @param id            id kategorie
 * @param name          jméno kategorie
 * @returns {string}    HTML kód položky
 */
function makeCategoryOption(id, name) {
    return `<option value="${id}">${name}</option>"`
}

/**
 * Vytvoří dialogové okno pro výběr kategorie a poté pomocí AJAX balíče kdo kategorie přidá.
 * @param find_code     vyhledávací kód balíčku
 */
function selectCategory(find_code) {
    Swal.fire({
      title: 'Vyberte kategorii',
      html: `<select class="form-select bg-secondary text-light" id="categorySelect">
                <option value="" selected disabled>Načítám kategorie...</option>
            </select>`,
      showCancelButton: true,
        confirmButtonText: 'Přidat',
        cancelButtonText: 'Zrušit',
        customClass: {
            confirmButton: "btn btn-success mx-2",
            cancelButton: "btn btn-danger mx-2"
        },
        buttonsStyling: false,
      didOpen() {
        $.ajax(categories_endpoint, {
            data: {
                find_code: find_code
            },
            success: (data, status, xhr) => {
                catselect = $('#categorySelect');
                catselect.html('<option value="" selected disabled>Vyberte kategorii...</option>');
                data.forEach((elem) => {
                    catselect.append(makeCategoryOption(elem._id, elem.name));
                });

          }
        })
      },
    }).then((result) => {
        if (result.isConfirmed) {
            $.ajax(add_category_endpoint, {
                method: "POST",
                data: {
                    category: $('#categorySelect option:selected').val(),
                    find_code: find_code
                },
                success: (data, status, xhr) => {
                    toast.fire({
                        title: "Kategorie přidána!",
                        icon: "success"
                    });
                },
                error: (xhr, status, error) => {
                    toast.fire({
                       title: "Chyba při přidávání kategorie!",
                       icon: "error"
                    });
                }
            })
        }
    })
}

function fav(find_code, newfav) {
    let endp = del_fav_endpoint;
    if (newfav) {
        endp = add_fav_endpoint;
    }
    $.ajax(endp, {
        method: "POST",
        data: {
            find_code: find_code
        },
        success: (data, status, xhr) => {
            let title = "Balíček přidán do oblíbených!";
            if (!newfav) {
                title = "Balíček byl odebrán z oblíbených!";
            }
            toast.fire({
               title: title,
                icon: "success"
            });
            let favbtn = $('#fav-'+find_code);
            favbtn.attr("onclick", `fav('${find_code}', ${!newfav})`);
            if (newfav) {
                favbtn.html('<span class="material-icons">favorite</span>');
            } else {
                favbtn.html('<span class="material-icons">favorite_border</span>');
            }
        },
        error: (xhr, status, error) => {
            let title = "Balíček se nepodařilo přidat mezi oblíbené!";
            if (!newfav) {
                title = "Balíček se nepodařilo odebrat z oblíbených!";
            }
            toast.fire({
               title: "Balíček se nepodařilo přidat mezi oblíbené!",
                icon: "error"
            });
        }
    })
}

search();