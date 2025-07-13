package com.homework.homework1.service.dto;

import org.springframework.cglib.core.Local;

import java.time.LocalDateTime;
import java.util.Date;

public class WeatherReport {

    private double temperature;
    private String state;
    private String city;
    private LocalDateTime date;

    public WeatherReport(double temperature, String state, String city, LocalDateTime date) {
        this.temperature = temperature;
        this.state = state;
        this.city = city;
        this.date = date;
    }

    public double getTemperature() {
        return temperature;
    }

    public void setTemperature(double temperature) {
        this.temperature = temperature;
    }

    public String getState() {
        return state;
    }

    public void setState(String state) {
        this.state = state;
    }

    public String getCity() {
        return city;
    }

    public void setCity(String city) {
        this.city = city;
    }

    public LocalDateTime getDate() {
        return date;
    }

    public void setDate(LocalDateTime date) {
        this.date = date;
    }
}
