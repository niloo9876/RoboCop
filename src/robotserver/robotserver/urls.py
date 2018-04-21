"""robotserver URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/2.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path

from mapping import views as views_mapping

urlpatterns = [
    #path('admin/', admin.site.urls),
    path('', views_mapping.home, name='home'),
    path('API/start_mapping', views_mapping.start_mapping, name='start_mapping'),
    path('API/stop_mapping', views_mapping.stop_mapping, name='stop_mapping'),
    path('API/start_patrolling', views_mapping.start_patrolling, name='start_patrolling'),
    path('API/stop_patrolling', views_mapping.stop_patrolling, name='stop_patrolling'),
    path('API/patrol_path', views_mapping.patrol_path, name='patrol_path'),
    path('API/clear_patrol_path', views_mapping.clear_patrol_path, name='clear_patrol_path'),
    path('API/boundaries', views_mapping.boundaries, name='boundaries'),
    path('API/current_location', views_mapping.current_location, name='current_location'),
    path('API/current_target', views_mapping.current_target, name='current_target'),
    path('API/current_mode', views_mapping.current_mode, name='current_mode'),
    path('API/notifications', views_mapping.notifications, name='notifications'),
    path('API/add_notification', views_mapping.add_notification, name='add_notification'),
    path('API/picture_request', views_mapping.picture_request, name='picture_request'),
    #path('API/song_request', views_mapping.song_request, name='song_request'),   
]
