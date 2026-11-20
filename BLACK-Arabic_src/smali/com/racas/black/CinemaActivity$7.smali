.class Lcom/racas/black/CinemaActivity$7;
.super Ljava/lang/Object;
.source "CinemaActivity.java"

# interfaces
.implements Landroid/widget/SeekBar$OnSeekBarChangeListener;


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

    .line 109
    iput-object p1, p0, Lcom/racas/black/CinemaActivity$7;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    return-void
.end method


# virtual methods
.method public onProgressChanged(Landroid/widget/SeekBar;IZ)V
    .locals 0

    .line 112
    return-void
.end method

.method public onStartTrackingTouch(Landroid/widget/SeekBar;)V
    .locals 1

    .line 110
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$7;->this$0:Lcom/racas/black/CinemaActivity;

    const/4 v0, 0x1

    invoke-static {p1, v0}, Lcom/racas/black/CinemaActivity;->access$602(Lcom/racas/black/CinemaActivity;Z)Z

    return-void
.end method

.method public onStopTrackingTouch(Landroid/widget/SeekBar;)V
    .locals 1

    .line 111
    iget-object v0, p0, Lcom/racas/black/CinemaActivity$7;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v0

    if-eqz v0, :cond_0

    iget-object v0, p0, Lcom/racas/black/CinemaActivity$7;->this$0:Lcom/racas/black/CinemaActivity;

    invoke-static {v0}, Lcom/racas/black/CinemaActivity;->access$200(Lcom/racas/black/CinemaActivity;)Landroid/widget/VideoView;

    move-result-object v0

    invoke-virtual {p1}, Landroid/widget/SeekBar;->getProgress()I

    move-result p1

    invoke-virtual {v0, p1}, Landroid/widget/VideoView;->seekTo(I)V

    :cond_0
    iget-object p1, p0, Lcom/racas/black/CinemaActivity$7;->this$0:Lcom/racas/black/CinemaActivity;

    const/4 v0, 0x0

    invoke-static {p1, v0}, Lcom/racas/black/CinemaActivity;->access$602(Lcom/racas/black/CinemaActivity;Z)Z

    return-void
.end method
