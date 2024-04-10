package api

import (
    "net/http"
    "time"
)

type Client struct {
    httpClient *http.Client
    baseURL    string
}

func NewClient(baseURL string) *Client {
    return &Client{
        baseURL:    baseURL,
        httpClient: &http.Client{Timeout: 10 * time.Second},
    }
}

// Example method to fetch debugging info from StoreServer
func (c *Client) FetchDebugInfo() (string, error) {
    resp, err := c.httpClient.Get(c.baseURL + "/debug/info")
    if err != nil {
        return "", err
    }
    defer resp.Body.Close()

    // Process the response as needed
    return "Processed data here", nil
}
