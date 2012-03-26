#!/usr/bin/env bash

# This script pulls translation files from Transifex
# Files are stored there in two big files, one for xivoclient, one for baselib
# We duplicate the big files for every .ts file we need, then we use lupdate to
# strip them down.

# To add a new translation :
# find . -name '*.pro' -exec sed -i -e 's|^TRANSLATIONS += $${\?ROOT_DIR}\?/i18n/\(.*\)_en.ts|\0\nTRANSLATIONS += $$ROOT_DIR/i18n/\1_it.ts|' {} \;
# find . -name '*.qrc' -exec sed -i -e 's|^\\( *\\)<file>\\(.*\\)obj/\\(.*\\)_fr.qm</file>|\\0\\n\\1<file>\\2obj/\\3_de.qm</file>|' {} \\;

LOCALES="en fr it de nl ja"

function usage {
    echo "Usage : $0 [help|pull|update]"
    echo
    echo "help : display this message"
    echo "pull : get translations from Transifex"
    echo "push : upload transltations to Transifex"
    echo "update : update translation files from source"
}

function pull_from_transifex {
    if [ ! -d .tx ]
    then
        tx init --host https://www.transifex.net
    fi

    tx set --auto-remote  https://www.transifex.net/projects/p/xivo/
    tx pull -r xivo.xivo-client -a
}

function find_all_ts_files {
    locale="$1"
    find . \( -path './baselib/*'    -a -name "*_$locale.ts" \) \
        -o \( -path './xivoclient/*' -a -name "*_$locale.ts" \) -print
}

function copy_from_transifex_to_git {
    # locale takes values as "fr.ts", "de.ts", "nl.ts" ...
    for locale in $(ls translations/xivo.xivo-client)
    do
        for ts in $(find_all_ts_files "en")
        do
            cp "translations/xivo.xivo-client/$locale" "${ts%en.ts}$locale"
        done
    done

    unmerge_translations
}

function lupdate_all {
    lupdate xivoclient/*.pro -no-obsolete
    lupdate baselib/baselib.pro -no-obsolete
}

function unmerge_translations {
    lupdate_all
}

function update_translations_from_source {
    lupdate_all
}

function merge_translations {
    if hash lconvert
    then
        for locale in ${LOCALES}
        do
            rm "xivoclient/i18n/all_$locale.ts"
            find_all_ts_files "$locale" \
            | xargs lconvert -o "xivoclient/i18n/all_$locale.ts"
        done
    fi
}

function push_to_transifex {
    tx set --auto-local -r xivo.xivo-client 'xivoclient/i18n/all_<lang>.ts' \
        --source-lang en --execute
    tx push -st -r xivo.xivo-client
}

function process_arguments {
    if [ $# -eq 0 ]
    then
        usage
        exit 1
    fi

    case $1 in
        pull)
            pull_from_transifex
            copy_from_transifex_to_git
            ;;
        push)
            merge_translations
            push_to_transifex
            ;;
        update)
            update_translations_from_source
            ;;
        *)
            usage
            exit 1
            ;;
    esac
}

process_arguments $@
