import pygame

pygame.init()
pygame.mixer.init()

mysound = pygame.mixer.sound("myFile.wav")

mysound.play()

#mysound.stop()