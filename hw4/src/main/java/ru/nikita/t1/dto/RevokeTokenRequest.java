package ru.nikita.t1.dto;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;

@AllArgsConstructor
@Getter
@Setter
public class RevokeTokenRequest {
    private String token;
}
