# From the Compton checkout: source halla.csh [build ROOT_PREFIX].
# Bash performs the shared build/setup; source its settings in the caller.
set _halla_ok = 0
set _halla_env = `mktemp`
if ($status != 0) goto halla_done
bash "$cwd/halla.sh" --csh $argv:q >! "$_halla_env"
if ($status == 0) then
    source "$_halla_env"
    if ($status == 0) set _halla_ok = 1
endif
/bin/rm -f "$_halla_env"
halla_done:
unset _halla_env
eval "unset _halla_ok; /bin/test $_halla_ok = 1"
