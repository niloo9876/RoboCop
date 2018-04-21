import requests

r = requests.get("http://griin.today/API/boundaries")
print(r.content)
requests.post("http://griin.today/API/boundaries", data={'x'})

