.class Lcom/racas/black/CinemaActivity$11;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/media/MediaPlayer$OnCompletionListener;


# annotations
.annotation system Ldalvik/annotation/EnclosingMethod;
    value = Lcom/racas/black/CinemaActivity;->startMovie(II)V
.end annotation

.annotation system Ldalvik/annotation/InnerClass;
    accessFlags = 0x0
    name = null
.end annotation


# instance fields
.field final synthetic this$0:Lcom/racas/black/CinemaActivity;


# direct methods
.method constructor <init>(Lcom/racas/black/CinemaActivity;)V
    .locals 0
    .annotation system Ldalvik/annotation/MethodParameters;
        accessFlags = {
            0x8010
        }
        names = {
            null
        }
    .end annotation

    .line 126
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$11;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onCompletion(Landroid/media/MediaPlayer;)V
    .locals 0

    .line 126
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$11;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {p1}, Lcom/racas/black/CinemaActivity;->access$100(Lcom/racas/black/CinemaActivity;)V

    return-void
.end method
