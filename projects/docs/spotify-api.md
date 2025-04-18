# Spotify API Integration Documentation

This document provides information on how to access and use the Spotify API integration in our home application server.

## Server Configuration

- **Default Port**: 3000
- **Base URL**: http://localhost:3000 (when running locally)
- **Authentication**: The API is designed to be used within a local network without strict authentication requirements.

## Authentication Flow

The application uses Spotify's OAuth2 flow for authentication:

1. User navigates to the login endpoint
2. User authenticates with Spotify
3. Spotify redirects back to our application with an authorization code
4. The application exchanges the code for access and refresh tokens
5. Authentication data is stored persistently (survives server restarts)

## API Endpoints

### Authentication Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/spotify/login` | GET | Redirect to Spotify authorization page |
| `/spotify/callback` | GET | Callback URL for Spotify (handled automatically) |
| `/spotify/logout` | GET | Log out the current user and clear stored tokens |
| `/spotify/status` | GET | Check if a user is authenticated with Spotify |

### Player Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/spotify/now-playing` | GET | Get information about the currently playing track |
| `/spotify/playback/play` | PUT | Resume playback |
| `/spotify/playback/pause` | PUT | Pause playback |
| `/spotify/skip/next` | POST | Skip to the next track |
| `/spotify/skip/previous` | POST | Skip to the previous track |
| `/spotify/me` | GET | Get the current user's Spotify profile information |

## Response Formats

### Now Playing Response

```json
{
  "isPlaying": true,
  "title": "Song Title",
  "artists": ["Artist 1", "Artist 2"],
  "album": "Album Name",
  "coverUrl": "https://full-resolution-image-url",
  "coverSmall": "data:image/jpeg;base64,...", // Base64 encoded 32x32px image
  "duration": 180000, // In milliseconds
  "progress": 45000, // In milliseconds
  "id": "spotify:track:id"
}
```

If nothing is currently playing:

```json
{
  "isPlaying": false
}
```

### Authentication Status Response

When authenticated:

```json
{
  "isLoggedIn": true,
  "user": {
    "id": "spotify-user-id",
    "name": "User Display Name",
    "email": "user@example.com"
  }
}
```

When not authenticated:

```json
{
  "isLoggedIn": false
}
```

### Playback Control Responses

Success:

```json
{
  "success": true,
  "action": "play"
}
```

Failure:

```json
{
  "error": "Error message"
}
```

## Example Usage

### JavaScript Fetch Examples

Check if user is logged in:

```javascript
fetch('/spotify/status')
  .then(response => response.json())
  .then(data => {
    if (data.isLoggedIn) {
      console.log(`Logged in as ${data.user.name}`);
    } else {
      console.log('Not logged in');
    }
  });
```

Get currently playing track:

```javascript
fetch('/spotify/now-playing')
  .then(response => response.json())
  .then(data => {
    if (data.isPlaying) {
      console.log(`Now playing: ${data.title} by ${data.artists.join(', ')}`);
    } else {
      console.log('Nothing is playing');
    }
  });
```

Control playback:

```javascript
// Play
fetch('/spotify/playback/play', { method: 'PUT' })
  .then(response => response.json())
  .then(data => {
    if (data.success) console.log('Playback started');
  });

// Pause
fetch('/spotify/playback/pause', { method: 'PUT' })
  .then(response => response.json())
  .then(data => {
    if (data.success) console.log('Playback paused');
  });

// Skip to next track
fetch('/spotify/skip/next', { method: 'POST' })
  .then(response => response.json())
  .then(data => {
    if (data.success) console.log('Skipped to next track');
  });

// Skip to previous track
fetch('/spotify/skip/previous', { method: 'POST' })
  .then(response => response.json())
  .then(data => {
    if (data.success) console.log('Skipped to previous track');
  });
```

## Authentication Persistence

The system is designed to persist authentication between server restarts. Spotify tokens are stored in a local file (`data/spotifyAuth.json`). This means:

1. You only need to authenticate once
2. The server will automatically refresh tokens when needed
3. Authentication survives server restarts

## Web Interface

A simple web interface is available at the root URL (`http://localhost:3000/`) which provides a user-friendly way to:

1. Login to Spotify
2. View currently playing track
3. Control playback (play/pause, next/previous)
4. View playback progress

## Environment Variables

The following environment variables must be configured:

```
spotify_client_id=YOUR_SPOTIFY_CLIENT_ID
spotify_secret=YOUR_SPOTIFY_CLIENT_SECRET
spotify_redirect_uri=http://localhost:3000/spotify/callback
```

These can be set in a `.env` file in the root of the application.