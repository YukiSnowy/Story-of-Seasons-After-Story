cd C:\Users\Pokpong\Desktop\Story-of-Seasons-After-Story

:loop
	git add .
	git commit -m "test"
	git push
	timeout /t 30 /nobreak
	goto :loop

:continue