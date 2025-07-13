package com.homework.homework1;

import jakarta.annotation.PostConstruct;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.event.ContextRefreshedEvent;
import org.springframework.context.event.EventListener;

@SpringBootApplication
public class Homework1Application {

	public static void main(String[] args) {
		SpringApplication.run(Homework1Application.class, args);
	}
}
