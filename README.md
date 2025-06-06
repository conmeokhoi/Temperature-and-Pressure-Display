## To use code, type following in terminal:
```
gcc TEMP_PRES.c -o TEMP_PRES -lwiringPi -lm
```
then make crontab
```
cronetab -e
```
choose nano if be asked, enter the following code in final line
```
@reboot /home/pi/startup.sh
```
Make sure to download file startup.sh and store in /home/pi

Demo video: https://youtu.be/Hr0xIVP4JFo

