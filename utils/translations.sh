#!/usr/bin/env bash

# This script pulls translation files from Transifex
# Files are stored there in two big files, one for xivoclient, one for baselib
# We duplicate the big files for every .ts file we need, then we use lupdate to
# strip them down.

# To add a new translation :
# find . -name '*.pro' -exec sed -i -e 's|^TRANSLATIONS += $${\?ROOT_DIR}\?/i18n/\(.*\)_en.ts|\0\nTRANSLATIONS += $$ROOT_DIR/i18n/\1_it.ts|' {} \;
# find . -name '*.qrc' -exec sed -i -e 's|^\( *\)<file>\(.*\)obj/\(.*\)_fr.qm</file>|\0\n\1<file>\2obj/\3_de.qm</file>|' {} \;

THIS_SCRIPT_DIRECTORY="$(dirname $0)"
XIVO_CLIENT_ROOT="$THIS_SCRIPT_DIRECTORY/.."

LOCALES_LIST="en fr it de nl ja hu pt_BR es_ES"

XIVO_CLIENT_RESOURCE=xivo.xivo-client
TRANSIFEX_URL=https://www.transifex.com
TRANSIFEX_PROJECT_URL=https://www.transifex.com/projects/p/xivo/resource/xivo-client
TRANSIFEX_I18N_DIR="$XIVO_CLIENT_ROOT/translations/xivo.xivo-client"
XIVO_CLIENT_I18N_DIR="$XIVO_CLIENT_ROOT/xivoclient/i18n"


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
    echo "Usage : $0 [help|pull|commit|push]"
    echo
    echo "help : display this message"
    echo "pull : get translations from Transifex"
    echo "commit : update translation files from source code, ready to push"
    echo "push : upload translations to Transifex"
}


function qmake_is_not_run {
    file_path="$XIVO_CLIENT_ROOT/versions.mak"
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
        push)
            push_english_to_transifex
            ;;
        commit)
            update_translations_from_source
            merge_translations
            ;;
        *)
            usage
            exit 1
            ;;
    esac
}


function initialize_transifex_if_needed {
    if [ ! -d .tx ]
    then
        tx init --host "$TRANSIFEX_URL"
    fi
    tx set --auto-remote "$TRANSIFEX_PROJECT_URL"
}


function pull_from_transifex {
    initialize_transifex_if_needed
    tx pull --resource "$XIVO_CLIENT_RESOURCE" --all --force --source
}


function prepare_transifex_for_pushing_english {
    mkdir -p "$TRANSIFEX_I18N_DIR"
    cp "$XIVO_CLIENT_I18N_DIR"/all_en.ts "$TRANSIFEX_I18N_DIR"/en.ts
    tx set --source --resource "$XIVO_CLIENT_RESOURCE" --language en "$TRANSIFEX_I18N_DIR"/en.ts
}


function push_english_to_transifex {
    initialize_transifex_if_needed
    prepare_transifex_for_pushing_english
    tx push --source --resource "$XIVO_CLIENT_RESOURCE"
}


function copy_from_transifex_to_git {
    for locale in ${LOCALES_LIST}
    do
        for ts in $(find_all_ts_files "en")
        do
            cp "$TRANSIFEX_I18N_DIR/$locale.ts" "${ts%en.ts}${locale}.ts"
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
        for locale in ${LOCALES_LIST}
        do
            rm "$XIVO_CLIENT_I18N_DIR/all_$locale.ts" -f
            find_all_ts_files "$locale" \
            | xargs lconvert -o "$XIVO_CLIENT_I18N_DIR/all_$locale.ts"
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
