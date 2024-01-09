cd C:\Users\Pokpong\Desktop\Story-of-Seasons-After-Story

:loop
	cd !kawaii-anticheat
	git pull
	cd ..
	git add .
	git commit -m "test"
	git push
	timeout /t 30 /nobreak
	goto :loop

:continue