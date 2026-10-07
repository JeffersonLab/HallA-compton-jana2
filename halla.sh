#!/usr/bin/env bash
# From the Compton checkout: source halla.sh [build ROOT_PREFIX].
# --csh emits settings for the thin halla.csh wrapper.
_halla_setup() {
    local emit_csh=false
    if [[ "${1:-}" == --csh ]]; then emit_csh=true; shift; fi
    if [[ $# -gt 2 || ( $# -gt 0 && "$1" != build ) ]]; then
        echo "Usage: source halla.sh [build ROOT_PREFIX]" >&2
        return 1
    fi
    if [[ "${1:-}" == build && ( $# -ne 2 || -z "$2" ) ]]; then
        echo "ROOT prefix is required: source halla.sh build /path/to/ROOT (or source halla.csh build /path/to/ROOT)." >&2
        return 1
    fi
    local root="$PWD" jce_source="${JCE_SOURCE_DIR-$PWD/../jana2-common-extensions}"
    if [[ ! -f "$root/halla.sh" || ! -f "$root/CMakeLists.txt" ]]; then
        echo "Run from the HallA-compton-jana2 checkout." >&2
        return 1
    fi
    if [[ -z "$jce_source" || ( ${JCE_SOURCE_DIR+x} && ! -f "$jce_source/superbuild/CMakeLists.txt" ) ]]; then
        echo "JCE_SOURCE_DIR must point to an existing jana2-common-extensions checkout, or be unset." >&2
        return 1
    fi
    if [[ -d "$jce_source" ]]; then
        jce_source=$(cd -- "$jce_source" && pwd) || return 1
    elif [[ "$jce_source" != /* ]]; then
        jce_source="$root/$jce_source"
    fi
    local stack="${JCE_HOME:-$jce_source/jce-stack}" install="$root"
    local prefixes="${CMAKE_PREFIX_PATH//:/;}"
    if [[ "$stack" != /* ]]; then stack="$root/$stack"; fi
    if [[ "${1:-}" == build ]]; then
        local root_config="" candidate
        for candidate in "$2/cmake" "$2/lib/cmake/ROOT" "$2/lib64/cmake/ROOT" "$2"; do
            if [[ -f "$candidate/ROOTConfig.cmake" ]]; then
                root_config=$(cd -- "$candidate" && pwd) || return 1
                break
            fi
        done
        if [[ -z "$root_config" ]]; then
            echo "ROOTConfig.cmake not found under the supplied ROOT prefix: $2" >&2
            return 1
        fi
        prefixes="$2${prefixes:+;$prefixes}"
        if [[ ! -d "$jce_source" ]]; then
            git clone https://github.com/JeffersonLab/jana2-common-extensions.git "$jce_source" >&2 || return 1
        fi
        if [[ ! -f "$jce_source/superbuild/CMakeLists.txt" ]]; then
            echo "Set JCE_SOURCE_DIR to an existing jana2-common-extensions checkout." >&2
            return 1
        fi
        cmake -S "$jce_source/superbuild" -B "$jce_source/build-super" \
            -DCMAKE_INSTALL_PREFIX="$stack" >&2 || return 1
        cmake --build "$jce_source/build-super" --parallel >&2 || return 1
        cmake -S "$root" -B "$root/build" \
            -DCMAKE_PREFIX_PATH="$stack${prefixes:+;$prefixes}" \
            -DROOT_DIR="$root_config" \
            -DCMAKE_INSTALL_PREFIX="$install" >&2 || return 1
        cmake --build "$root/build" --parallel >&2 || return 1
        cmake --install "$root/build" >&2 || return 1
    fi
    if [[ ! -x "$stack/bin/jana" || ! -d "$install/lib/plugins" || ! -d "$install/config" ]]; then
        echo "JCE or Compton is not installed. Run: source halla.sh build (or source halla.csh build)." >&2
        return 1
    fi
    local plugins="${JANA_PLUGIN_PATH:-}" configs="${JCE_CONFIG_DIR:-}"
    case ":$plugins:" in
        *":$install/lib/plugins:"*) ;;
        *) plugins="$install/lib/plugins${plugins:+:$plugins}" ;;
    esac
    case ":$configs:" in
        *":$install/config:"*) ;;
        *) configs="${configs:+$configs:}$install/config" ;;
    esac
    if $emit_csh; then
        # Single-quoted settings are data, including paths with spaces/quotes.
        local name value
        for name in JCE_HOME COMPTON_HOME JANA_PLUGIN_PATH JCE_CONFIG_DIR; do
            case "$name" in
                JCE_HOME) value=$stack ;;
                COMPTON_HOME) value=$install ;;
                JANA_PLUGIN_PATH) value=$plugins ;;
                JCE_CONFIG_DIR) value=$configs ;;
            esac
            value=${value//\'/\'\\\'\'}
            printf "setenv %s '%s'\n" "$name" "$value"
        done
    else
        export JCE_HOME="$stack" COMPTON_HOME="$install"
        export JANA_PLUGIN_PATH="$plugins" JCE_CONFIG_DIR="$configs"
    fi
}
if _halla_setup "$@"; then
    unset -f _halla_setup
    true
else
    unset -f _halla_setup
    false
fi
