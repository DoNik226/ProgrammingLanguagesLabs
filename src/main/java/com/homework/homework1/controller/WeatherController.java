package com.homework.homework1.controller;

import com.homework.homework1.service.WeatherProducer;
import com.homework.homework1.service.dto.WeatherReport;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
@RequestMapping("/weather")
public class WeatherController {
    private WeatherProducer weatherProducer;

    public WeatherController(WeatherProducer weatherProducer) {
        this.weatherProducer = weatherProducer;
    }

    @PostMapping
    public void getWeatherReport(@RequestBody int cityId) {
        weatherProducer.sendWeatherReport(cityId);
    }
}
