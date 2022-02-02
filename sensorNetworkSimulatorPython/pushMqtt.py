# Created by Maarten Dequanter -  Erasmus Hogeschool Brussel
# This script will use the content of the file sensordata.csv to simulate incomming sensordata.
# The content is a real dataset of about 30000 records, measured from a sensornetwork of 10 devices over a period of multiple days
# The parameters measured are temperature, light and barometric pressure.
# Please feel free to use/adapt this code.

import time
import paho.mqtt.client as paho
import csv

broker="127.0.0.1"
#define callback
def on_message(client, userdata, message):
    time.sleep(1)
    print("received message =",str(message.payload.decode("utf-8")))
client= paho.Client("client1")
######Bind function to callback
client.on_message=on_message
#####
print("connecting to broker ",broker)
client.connect(broker)#connect
client.loop_start() #start loop to process received messages
print("subscribing ")
client.subscribe("sensordata")#subscribe
time.sleep(2)


filename = 'sensordata.csv'

rowCounter = 0

with open(filename, 'r') as csvfile:
    datareader = csv.reader(csvfile)
    for row in datareader:
        messageContent = "{\"name\":\""+row[1]+"\",\"light\":\""+row[4]+"\",\"temperature\":\""+row[2]+"\",\"pressure\":\""+row[3]+"\"}"
        rowCounter+=1
        print(messageContent)
        client.publish("sensordata", messageContent)
        time.sleep(5)
        if (rowCounter > 100000) :
            exit()


client.disconnect() #disconnect
client.loop_stop() #stop loop
