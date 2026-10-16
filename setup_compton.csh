# From the Compton source directory: source setup_compton.csh [build [ROOT_PREFIX]].
# Bash performs the shared build/setup; source its settings in the caller.
set _compton_ok = 0
set _compton_env = `mktemp`
if ($status != 0) goto compton_done
bash "$cwd/setup_compton.sh" --csh $argv:q >! "$_compton_env"
if ($status == 0) then
    source "$_compton_env"
    if ($status == 0) set _compton_ok = 1
endif
/bin/rm -f "$_compton_env"
compton_done:
unset _compton_env
eval "unset _compton_ok; /bin/test $_compton_ok = 1"
