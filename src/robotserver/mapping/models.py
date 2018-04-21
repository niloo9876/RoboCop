from django.db import models
import json

# Create your models here.

class Map(models.Model):
	NORMAL_MODE = 0
	MAPPING_MODE = 1
	PATROL_MODE = 2
	MODE_CHOICES = (
		(NORMAL_MODE, 'normal'),
		(MAPPING_MODE, 'mapping'),
		(PATROL_MODE, 'patrol'),
	)

	boundaries = models.TextField(default='[]') #enconded as json data
	patrol_path = models.TextField(default='[]') #enconded as json data
	current_location = models.TextField(default='{"x": 0, "y": 0, "angle": 90}') #encoded as json data
	current_target = models.TextField(default='{"x": null, "y": null}')
	current_mode = models.IntegerField(default=NORMAL_MODE, choices=MODE_CHOICES)
	images_requested = models.PositiveIntegerField(default=0)

	def get_parsed_boundaries(self):
		return json.loads(self.boundaries)

	def get_parsed_patrol_path(self):
		return json.loads(self.patrol_path)

	def get_parsed_location(self):
		return json.loads(self.current_location)

	def get_parsed_target(self):
		return json.loads(self.current_target)

	def add_boundary_point(self, x, y):
		boundaries = self.get_parsed_boundaries()
		boundaries.append({
				"x": float(x),
				"y": float(y)
			})

		self.boundaries = json.dumps(boundaries)

	def add_boundary_points(self, list_of_points):
		boundaries = self.get_parsed_boundaries()

		for point in list_of_points:
			boundaries.append({
					"x": float(point["x"]),
					"y": float(point["y"])
				})

		self.boundaries = json.dumps(boundaries)

	def add_patrol_points(self, list_of_points):
		#clear previous patrol points
		patrol_path = []
		points = json.loads(list_of_points)

		for point in points:
			patrol_path.append({
					"x": float(point[0]),
					"y": float(point[1])
			})

		self.patrol_path = json.dumps(patrol_path)

	def update_location(self, x, y, angle):
		current_location = {
			"x": float(x),
			"y": float(y),
			"angle": float(angle)
		}

		self.current_location = json.dumps(current_location)

	def update_target(self, x, y):
		current_target = {
			"x": float(x),
			"y": float(y)
		}

		self.current_target = json.dumps(current_target)


class Notification(models.Model):
	title = models.TextField(default="Notification")
	description = models.TextField(default="")
	img = models.TextField(null=True, blank=True)

