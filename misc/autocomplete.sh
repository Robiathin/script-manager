_sm() {
	local cur=${COMP_WORDS[COMP_CWORD]}
	local prev=${COMP_WORDS[COMP_CWORD-1]}

	case "$prev" in
	-e|-E|-r|-d|-V)
		COMPREPLY=( $( compgen -W "$(sm -C)" -- $cur ) )
		return 0
		;;
	esac
}
complete -F _sm sm
