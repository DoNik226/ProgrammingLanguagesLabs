package com.homework.homework1;

import org.springframework.context.event.ContextRefreshedEvent;
import org.springframework.context.event.EventListener;

public class Main {
    public static void main(String[] args) {
        WeatherClient weatherClient = new WeatherClient();
        for (int i = 0; i < 3; i++) {
            weatherClient.sendWeatherRequest(i);
        }
    }
}
