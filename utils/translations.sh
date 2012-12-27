#!/usr/bin/env bash

# This script pulls translation files from Transifex
# Files are stored there in two big files, one for xivoclient, one for baselib
# We duplicate the big files for every .ts file we need, then we use lupdate to
# strip them down.

# To add a new translation :
# find . -name '*.pro' -exec sed -i -e 's|^TRANSLATIONS += $${\?ROOT_DIR}\?/i18n/\(.*\)_en.ts|\0\nTRANSLATIONS += $$ROOT_DIR/i18n/\1_it.ts|' {} \;
# find . -name '*.qrc' -exec sed -i -e 's|^\( *\)<file>\(.*\)obj/\(.*\)_fr.qm</file>|\0\n\1<file>\2obj/\3_de.qm</file>|' {} \;

this_script_directory="$(dirname $0)"

LOCALES="en fr it de nl ja hu pt_BR es_ES"


function main {
    if [ $# -eq 0 ] ; then
        usage
        exit 1
    fi
    if qmake_is_not_run ; then
        echo "Please run qmake before updating translations."
        exit 2
    fi
    process_arguments $@
}


function usage {
    echo "Usage : $0 [help|pull|update]"
    echo
    echo "help : display this message"
    echo "pull : get translations from Transifex"
    echo "push : upload translations to Transifex"
    echo "update : update translation files from source"
}


function qmake_is_not_run {
    file_path="$this_script_directory/../versions.mak"
    if [ -r "$file_path" ] ; then
        return 1
    else
        return 0
    fi
}


function process_arguments {
    case $1 in
        pull)
            pull_from_transifex
            copy_from_transifex_to_git
            ;;
        update)
            update_translations_from_source
            merge_translations
            ;;
        *)
            usage
            exit 1
            ;;
    esac
}


function pull_from_transifex {
    if [ ! -d .tx ]
    then
        tx init --host https://www.transifex.net
    fi

    tx set --auto-remote  https://www.transifex.net/projects/p/xivo/
    tx pull -r xivo.xivo-client -a -s -f
}


function copy_from_transifex_to_git {
    # locale takes values as "fr.ts", "de.ts", "nl.ts" ...
    for locale in ${LOCALES}
    do
        for ts in $(find_all_ts_files "en")
        do
            cp "translations/xivo.xivo-client/$locale.ts" "${ts%en.ts}${locale}.ts"
        done
    done

    unmerge_translations
}


function update_translations_from_source {
    lupdate_all
}


function merge_translations {
    if hash lconvert
    then
        for locale in ${LOCALES}
        do
            rm "xivoclient/i18n/all_$locale.ts" -f
            find_all_ts_files "$locale" \
            | xargs lconvert -o "xivoclient/i18n/all_$locale.ts"
        done
    else
        echo "You need to install the Qt tool lconvert."
        exit 1
    fi
}


function unmerge_translations {
    lupdate_all
}


function find_all_ts_files {
    locale="$1"
    find baselib xivoclient -name "*_$locale.ts" -print
}


function lupdate_all {
    lupdate "baselib/baselib.pro" -no-obsolete
    lupdate "xivoclient/xletlib.pro" -no-obsolete
    lupdate "xivoclient/xivoclient.pro" -no-obsolete
    lupdate "xivoclient/xlets.pro" -no-obsolete
}


main $@
