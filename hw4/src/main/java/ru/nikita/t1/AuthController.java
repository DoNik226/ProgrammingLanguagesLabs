package ru.nikita.t1;

import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import ru.nikita.t1.dto.LoginRequest;
import ru.nikita.t1.dto.RefreshTokenRequest;
import ru.nikita.t1.dto.RegistrationRequest;
import ru.nikita.t1.dto.RevokeTokenRequest;

@RestController
public class AuthController {

    private final AuthenticationService authenticationService;

    public AuthController(AuthenticationService authenticationService) {
        this.authenticationService = authenticationService;
    }

    @PostMapping("/register")
    public ResponseEntity<?> register(@RequestBody RegistrationRequest request) {
        return authenticationService.register(request);
    }

    @PostMapping("/login")
    public ResponseEntity<?> authenticate(@RequestBody LoginRequest request) {
        return authenticationService.authenticate(request);
    }

    @PostMapping("/refresh-token")
    public ResponseEntity<?> refreshTokens(@RequestBody RefreshTokenRequest request) {
        return authenticationService.refreshToken(request);
    }

    @PostMapping("/revoke-token")
    public ResponseEntity<?> revokeToken(@RequestBody RevokeTokenRequest request) {
        return authenticationService.revokeToken(request);
    }
}
