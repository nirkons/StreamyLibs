from __future__ import print_function
import urllib2 as urlreq
from botocore.vendored import requests
from HTMLParser import HTMLParser
import re


#Variables
#Fill in your Netflix username and password here
USERNAME = ""
PASSWORD = ""

    # PLEASE CHANGE THE IP of cloud-blynk.com to your region's IP as blynk-cloud's servers are not connected
    # Europe -> 139.59.206.133    US -> 45.55.96.146    Middle east -> 188.166.206.43
    # To make sure the IP is correct, open your CMD/Terminal and ping blynk-cloud.com to see which IP you get
BLYNKIP = '188.166.206.43'

BLYNKAUTH = ''

authenticity_token = ''
lastwatchid = ''

#HTML Parser for getting the login page for video service
class LoginParser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.recording = 0
        self.data = []
    
    def handle_starttag(self, tag, attributes):
        global authenticity_token
        if tag == 'authURL':
            return
        if self.recording:
            self.recording += 1
            return
        for name, value in attributes:
            if value=='authURL':
                for value,authURL in attributes:
                    if ('value' in value):
                        authenticity_token = authURL
        else:
            return
        self.recording = 1
    
    def handle_endtag(self, tag):
        if tag == 'div' and self.recording:
            self.recording -= 1
    
    def handle_data(self, data):
        if self.recording:
            self.data.append(data)

#HTML Parser for scraping the last watched show from video service    
class lastwatchedparser(HTMLParser):
    def __init__(self):
        HTMLParser.__init__(self)
        self.recording = 0
        self.data = []

    def handle_starttag(self, tag, attributes):
        global lastwatchid
        if tag == 'div':
            return
        if self.recording:
            self.recording += 1
            return
        for name, value in attributes:
            if ('/title/' in value):
                lastwatchid += value
            else:
                return
            self.recording = 1

    def handle_endtag(self, tag):
        if tag == 'div' and self.recording:
            self.recording -= 1

    def handle_data(self, data):
        if self.recording:
            self.data.append(data)

# --------------- Helpers that build all of the responses ----------------------

def build_speechlet_response(title, output, reprompt_text, should_end_session):
    return {
        'outputSpeech': {
            'type': 'PlainText',
            'text': output
        },
        'card': {
            'type': 'Simple',
            'title': "SessionSpeechlet - " + title,
            'content': "SessionSpeechlet - " + output
        },
        'reprompt': {
            'outputSpeech': {
                'type': 'PlainText',
                'text': reprompt_text
            }
        },
        'shouldEndSession': should_end_session
    }


def build_response(session_attributes, speechlet_response):
    return {
        'version': '1.0',
        'sessionAttributes': session_attributes,
        'response': speechlet_response
    }


# --------------- Functions that control the skill's behavior ------------------

def get_welcome_response():
    """ If we wanted to initialize the session to have some attributes we could
    add those here
    """

    session_attributes = {}
    card_title = "Welcome"
    speech_output = "Welcome to Streamy"
    # If the user either does not reply to the welcome message or says something
    # that is not understood, they will be prompted again with this text.
    reprompt_text = "Would you like to stream something?"
    should_end_session = False
    return build_response(session_attributes, build_speechlet_response(
        card_title, speech_output, reprompt_text, should_end_session))


def handle_session_end_request():
    card_title = "Session Ended"
    speech_output = "Thanks for trying out Streamy"
    # Setting this to true ends the session and exits the skill.
    should_end_session = True
    return build_response({}, build_speechlet_response(
        card_title, speech_output, None, should_end_session))
              
def playintent(intent, session):
    card_title = intent['name']
    session_attributes = {}
    should_end_session = False
    serviceslot =  intent['slots']['service']['resolutions']['resolutionsPerAuthority'][0]['values'][0]['value']['name']
    locationslot = intent['slots']['location']['resolutions']['resolutionsPerAuthority'][0]['values'][0]['value']['name']
    if (serviceslot == None or serviceslot == ""):
        serviceslot = intent['slots']['service']['value']
    if (locationslot == None or locationslot == ""):
        locationslot = intent['slots']['location']['value']
    target = ""
    if (serviceslot == 'netflix'):
        
        LOGIN_URL = "https://www.netflix.com/login"
        URL = "https://www.netflix.com/WiViewingActivity"
        
        session_requests = requests.session()
        target = '1'
        
        # Get login csrf token
        result = session_requests.get(LOGIN_URL)
        parser = LoginParser()
        parser.feed(result.text)
    
        # Create payload
        payload = {
            "action": "loginAction",
            "authURL": authenticity_token,
            "email": USERNAME,
            "flow": "websiteSignUp",
            "mode": "login",
            "nextPage": "",
            "password": PASSWORD,
            "rememberMe": "true",
            "showPassword": "",
            "withFields": "email,password,rememberMe,nextPage,showPassword"
        }
    
        # Perform login
        result = session_requests.post(LOGIN_URL, data = payload, headers = dict(referer = URL))
    
        # Scrape url
        result = session_requests.get(URL, headers = dict(referer = LOGIN_URL))
    
        parser = lastwatchedparser()
        parser.feed(result.text)
        
        #Update blynk 
        lastwatchedurl = "http://"+BLYNKIP+"/"+BLYNKAUTH+"/update/V4?value=" + lastwatchid[7:15]
        
        #Method 1 of sending the request
        #lastreq = urlreq.Request(lastwatchedurl)
        #urlreq.urlopen(lastreq).read()
        
        #Method 2 of sending the request, I found this to be more reliable
        update_nfx = requests.session()
        nfxresult = update_nfx.get(lastwatchedurl)
    elif (serviceslot == 'spotify'):
        target = '2'
    elif (serviceslot == 'VLC'):
        target = '3'
    
    if (locationslot == 'living room'):
        target += '4'
    elif (locationslot == 'bedroom'):
        target += '5'
    elif (locationslot == 'PC'):
        target += '6'
        
    finalurl = "http://"+BLYNKIP+"/"+BLYNKAUTH+"/update/V3?value=" + target
    
    #Method 1 of sending the request
    #req = urlreq.Request(finalurl)
    #urlreq.urlopen(req).read()
    
    #Method 2 of sending the request
    updatefinalurl = requests.session()
    finalresults = updatefinalurl.get(finalurl)
    
    session_attributes = target
    speech_output = "Playing"
    reprompt_text = "You can say, play this file in my bedroom or play this music in my work room"
    return build_response(session_attributes, build_speechlet_response(
        card_title, speech_output, reprompt_text, should_end_session))

# --------------- Events ------------------

def on_session_started(session_started_request, session):
    """ Called when the session starts """

    print("on_session_started requestId=" + session_started_request['requestId']
          + ", sessionId=" + session['sessionId'])


def on_launch(launch_request, session):
    """ Called when the user launches the skill without specifying what they
    want
    """

    print("on_launch requestId=" + launch_request['requestId'] +
          ", sessionId=" + session['sessionId'])
    # Dispatch to your skill's launch
    return get_welcome_response()


def on_intent(intent_request, session):
    """ Called when the user specifies an intent for this skill """

    print("on_intent requestId=" + intent_request['requestId'] +
          ", sessionId=" + session['sessionId'])

    intent = intent_request['intent']
    intent_name = intent_request['intent']['name']

    # Dispatch to your skill's intent handlers
    if intent_name == "Play":
        return playintent(intent, session)
    elif intent_name == "AMAZON.HelpIntent":
        return get_welcome_response()
    elif intent_name == "AMAZON.CancelIntent" or intent_name == "AMAZON.StopIntent":
        return handle_session_end_request()
    else:
        raise ValueError("Invalid intent")


def on_session_ended(session_ended_request, session):
    """ Called when the user ends the session.

    Is not called when the skill returns should_end_session=true
    """
    print("on_session_ended requestId=" + session_ended_request['requestId'] +
          ", sessionId=" + session['sessionId'])
    # add cleanup logic here


# --------------- Main handler ------------------

def lambda_handler(event, context):
    """ Route the incoming request based on type (LaunchRequest, IntentRequest,
    etc.) The JSON body of the request is provided in the event parameter.
    """
    print("event.session.application.applicationId=" +
          event['session']['application']['applicationId'])

    """
    Uncomment this if statement and populate with your skill's application ID to
    prevent someone else from configuring a skill that sends requests to this
    function.
    """
    # if (event['session']['application']['applicationId'] !=
    #         "amzn1.echo-sdk-ams.app.[unique-value-here]"):
    #     raise ValueError("Invalid Application ID")

    if event['session']['new']:
        on_session_started({'requestId': event['request']['requestId']},
                           event['session'])

    if event['request']['type'] == "LaunchRequest":
        return on_launch(event['request'], event['session'])
    elif event['request']['type'] == "IntentRequest":
        return on_intent(event['request'], event['session'])
    elif event['request']['type'] == "SessionEndedRequest":
        return on_session_ended(event['request'], event['session'])