from requests import get
from sys import argv

def kelvinToCelsius(kelvin):
    return round(kelvin - 273.15,2)

if len(argv) != 3:
    print('usage: ./133694 key city')
    exit()


city = argv[2]
key = argv[1]
url = f'https://api.openweathermap.org/data/2.5/weather?q={city}&appid={key}'
res = get(url)

response  = res.json()
if res.status_code != 200:
    if str(response).find('city not found') != -1:
        print(f"The city name {city} is not found!")
    else:
        print('invaid key')

else:
    city = response['name']

    country = response['sys']['country']
    
    longitude, latitude = response['coord']['lon'], response['coord']['lat']
    
    weather = response['weather'][0]['main']
    
    temp = kelvinToCelsius(response['main']['temp'])

    
    print(
    f"""
    City name: {city}
    Country: {country}
    Longitude, Latitude: {longitude}, {latitude}
    Weather: {weather}
    Temperature: {temp} C
    """)
