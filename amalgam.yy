fs ∆ ≣'fs'

ignore ∆ ['stdio.h', 'wchar.h', 'string.h']
R ∆ []

go ('jslike.h')

➮ go {
	A ∆ ⛁ a ≂⌶'\n'
	i ⬌ A {
		⌥ (Aⁱ ≀ '#' == 0 && Aⁱ ≀ 'include' > 0) {
			s ∆ repl(repl(Aⁱ,'<','"'),'>','"')⌶'"'
			⌥ (ignore ≀ (s¹) >= 0) { R ⬊ (Aⁱ); ♻ }
			ロ 'include', s¹
			go(s¹)
		} ⎇ R ⬊ (Aⁱ)
	}
}
R ⬋ ''
R ⬋ '// you can just copy this single file to your project and include.'
R ⬋ '// This is amalgamated version of jslike.h.'
ロ R↥
⛃('_jslike.h', R⫴'\n')
