from django.shortcuts import render, redirect
from django.http import HttpResponse, JsonResponse
from mapping.models import Map, Notification

from django.views.decorators.csrf import csrf_exempt

# Create your views here.

def home(request):
	return render(request, 'mapping/index.html', {})

@csrf_exempt
def start_mapping(request):
	if request.method == 'POST':
		#get the latest map
		last_map = Map.objects.last()
		if last_map is None:
			return HttpResponse('ERROR: cannot start mapping mode if you have not created a map before')

		if last_map.current_mode != Map.NORMAL_MODE:
			return HttpResponse('ERROR: cannot start mapping mode if not in normal mode')

		#create a new map everytime you start mapping
		last_map = Map()
		last_map.current_mode = Map.MAPPING_MODE
		last_map.save()
		return HttpResponse('started mapping mode!')
	else:
		return HttpResponse('ERROR: cannot start mapping mode if GET request')

@csrf_exempt
def stop_mapping(request):
	if request.method == 'POST':
		#get the latest map
		last_map = Map.objects.last()
		if last_map is None:
			return HttpResponse('ERROR: cannot stop mapping mode if you have not created a map before')

		if last_map.current_mode != Map.MAPPING_MODE:
			return HttpResponse('ERROR: cannot stop mapping mode if not in mapping mode')

		last_map.current_mode = Map.NORMAL_MODE
		last_map.save()
		return JsonResponse({"boundaries": last_map.get_parsed_boundaries()})
	else:
		return HttpResponse('ERROR: cannot stop mapping mode if GET request')

@csrf_exempt
def start_patrolling(request):
	if request.method == 'POST':

		#get the latest map
		last_map = Map.objects.last()
		if last_map is None:
			return HttpResponse('ERROR: cannot start patrol mode if you have not created a map before')

		if last_map.current_mode != Map.NORMAL_MODE:
			return HttpResponse('ERROR: cannot start patrol mode if not in normal mode')

		last_map.current_mode = Map.PATROL_MODE
		
		#update the patrol path of the map if one was given
		#return JsonResponse(request.POST)

		if 'patrol_path' in request.POST:
			last_map.add_patrol_points(request.POST.get('patrol_path'))

		last_map.save()
		return HttpResponse('started patrol mode!')
	else:
		return HttpResponse('ERROR: cannot start patrol mode if GET request')

@csrf_exempt
def stop_patrolling(request):
	if request.method == 'POST':
		#get the latest map
		last_map = Map.objects.last()
		if last_map is None:
			return HttpResponse('ERROR: cannot stop patrol mode if you have not created a map before')

		if last_map.current_mode != Map.PATROL_MODE:
			return HttpResponse('ERROR: cannot stop patrol mode if not in patrol mode')

		last_map.current_mode = Map.NORMAL_MODE
		last_map.save()
		return HttpResponse("stopped patrol mode!")
	else:
		return HttpResponse('ERROR: cannot stop mapping mode if GET request')

@csrf_exempt
def boundaries(request):
	'''
		If POST request: check if in mapping mode, and add boundary if so
		If GET request: get current boundaries
	'''
	#get the latest map (if there is nothing, create it)
	last_map = Map.objects.last()
	if last_map is None:
		last_map = Map()
		last_map.save()

	if request.method == 'POST':
		if last_map.current_mode == Map.MAPPING_MODE:
			last_map.add_boundary_point(request.POST.get('x'), request.POST.get('y'))
			last_map.save()
			return HttpResponse('point added!')
		else:
			return HttpResponse('ERROR: not in mapping mode, so the point was not added!')

	else:
		return JsonResponse({"boundaries": last_map.get_parsed_boundaries()})

@csrf_exempt
def patrol_path(request):
	#get the latest map (if there is nothing, create it)
	last_map = Map.objects.last()
	if last_map is None:
		last_map = Map()
		last_map.save()

	return JsonResponse({"patrol_path": last_map.get_parsed_patrol_path()})

@csrf_exempt
def clear_patrol_path(request):
	if request.method == 'POST':
		#get the latest map (if there is nothing, create it)
		last_map = Map.objects.last()
		if last_map is None:
			last_map = Map()
			last_map.save()

		last_map.patrol_path = "[]"
		last_map.save()
		return HttpResponse("patrol path cleared!")
	else:
		return HttpResponse('ERROR: cannot stop mapping mode if GET request')

@csrf_exempt
def current_location(request):
	'''
		If POST request: check if in mapping mode, and add boundary if so
		If GET request: get current boundaries
	'''
	#get the latest map (if there is nothing, create it)
	last_map = Map.objects.last()
	if last_map is None:
		last_map = Map()
		last_map.save()

	if request.method == 'POST':
		last_map.update_location(request.POST.get('x'), request.POST.get('y'), request.POST.get('angle'))
		last_map.save()
		return HttpResponse('location updated')
	else:
		return JsonResponse({"current_location": last_map.get_parsed_location()})

@csrf_exempt
def current_target(request):
	'''
		If POST request: check if in mapping mode, and add boundary if so
		If GET request: get current boundaries
	'''
	#get the latest map (if there is nothing, create it)
	last_map = Map.objects.last()
	if last_map is None:
		last_map = Map()
		last_map.save()

	if request.method == 'POST':
		last_map.update_target(request.POST.get('x'), request.POST.get('y'))
		last_map.save()
		return HttpResponse('target updated')
	else:
		return HttpResponse(last_map.current_target, content_type="application/json")

@csrf_exempt
def current_mode(request):
	last_map = Map.objects.last()
	if last_map is None:
		last_map = Map()
		last_map.save()

	data = {
		"current_mode": last_map.current_mode
	}

	"""
	if last_map.current_mode == Map.NORMAL_MODE:
		data["mode_data"] = last_map.get_parsed_boundaries()
	elif last_map.current_mode == Map.PATROL_MODE:
		data["mode_data"] = last_map.get_parsed_patrol_path()
	"""

	return JsonResponse(data)



@csrf_exempt
def notifications(request):
	notifications = Notification.objects.all().order_by("-pk")

	if request.method == "DELETE":
		notifications.delete()
		#now add a fixed notification
		notification = Notification(
				title="Welcome to Robocop",
				description="Hope you enjoy the app!",
				img="https://cdn.pastemagazine.com/www/articles/RoboCop%20Poster%20Main.jpg"
			)
		notification.save()
		return HttpResponse("notifications deleted!")

	entries = []

	#go through every notification
	for notification in notifications:
		#get the entry 
		entry = {
			"id": notification.pk,
			"title": notification.title,
			"description": notification.description
		}

		#since the img url is optional, check if it is there before adding it
		if notification.img is not None:
			entry["img"] = notification.img

		#add the entry
		entries.append(entry)

	return JsonResponse({
			'entries': entries
		})

@csrf_exempt
def add_notification(request):
	#only allow people to add notifications in POST
	if request.method == 'POST':
		#create the notification
		notification = Notification(
				title=request.POST.get('title'),
				description=request.POST.get('description'),
			)

		#add image if available
		if 'img' in request.POST:
			notification.img = img = request.POST.get('img')

		notification.save()
		return HttpResponse("notification added!")
	else:
		return HttpResponse("ERROR: can't add notification in GET request")


@csrf_exempt
def picture_request(request):
	last_map = Map.objects.last()
	if last_map is None:
		last_map = Map()
		last_map.save()

	#only allow people to add requests in POST
	if request.method == 'POST':
		last_map.images_requested = last_map.images_requested + 1
		last_map.save()
		return HttpResponse("picture request added!")

	#else, get how many requests we have so far and send true if more than 0. Also, decrease the size if so
	else:
		data = {
			"image_requested": False
		}

		if last_map.images_requested > 0:
			last_map.images_requested = last_map.images_requested - 1
			last_map.save()
			data["image_requested"] = True

		return JsonResponse(data)
