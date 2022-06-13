# EEEROVER
This is code for Summer Project for group CPA


The goal of the code is to achieve real time wireless communication between Rover and users, and also display processed Rover collected data on the rocks. 

There are various wireless communication protocals. Initially I thought about Bluetooth, as it will have minimal delay for short range communication, and that is what most controllers use. But it seems rather absurd as bluetooth will not be able to reach to the moon. Radio is also an option, but it will require extra hardware for the Computer side to read the information. My personal exprience with radio emmitter/recievers is that they will use a lot of pins and the connection is not very reliable. Therefore it seems nessassary to approach using WIFI.

We first thought about having the Rover as the host of the server. We had different approaches, first we tried to have the rover acting as a (AP or AT) Access point, where it is the host of the WIFI. When we connect to the dedicated IP it will send the HTML code to your device, therefore displaying the webpage. We quickly discarded this approach because coding Javascript and HTML for the rover to send line by line is laborious and inefficient. The HTML is resent every header we go to(for example, clicking a button), and will cause long delays if the html code is long. A similar approach, the one on the demo connects to the school EEEROVER network and hosts a server over LAN. But we discarded that one as well becuase firstly using the same public network with everyone else is insecure, and secondly we found out the memory of the arduino board is not large enough to hold long javascript code, and the website will not be displayed. A possible solution could be inserting a SD card and have the webpage stored therer, but I do not like the idea personally.


Server side:

HTML client side:

