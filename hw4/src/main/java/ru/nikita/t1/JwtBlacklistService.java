package ru.nikita.t1;

import io.jsonwebtoken.Claims;
import org.springframework.stereotype.Service;

import java.util.HashSet;
import java.util.Set;

@Service
public class JwtBlacklistService {

    private final Set<Claims> blacklistedTokens = new HashSet<>();

    public void addToBlacklist(Claims claims) {
        blacklistedTokens.add(claims);
    }

    public boolean isRevoked(Claims claims) {
        return blacklistedTokens.contains(claims);
    }
}
